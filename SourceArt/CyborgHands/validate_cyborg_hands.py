"""Reopen and verify the saved rig; write evidence beside the asset."""
import bpy, os, json, math
from mathutils import Vector

OUT=os.path.dirname(os.path.abspath(__file__))
bpy.ops.wm.open_mainfile(filepath=os.path.join(OUT,'CyborgHands.blend'))
scene=bpy.context.scene
rig=bpy.data.objects['CyborgHands_Rig']
meshes=[bpy.data.objects[n] for n in ['CyborgHands_R | articulated armor','CyborgHands_L | articulated armor']]
report={'blender':bpy.app.version_string,'reopened_saved_file':True,'bone_count':len(rig.data.bones),'meshes':[],'pose_tests':[]}
assert len(rig.data.bones)==39
assert scene.unit_settings.system=='METRIC' and scene.unit_settings.scale_length==1
for side in ['l','r']:
    for digit in ['thumb','index','middle','ring','pinky']:
        for i in range(1,4):assert f'{digit}_{i:02d}_{side}' in rig.data.bones
for obj in meshes:
    obj.data.calc_loop_triangles()
    bad=[]; weighted={}
    for v in obj.data.vertices:
        assert all(math.isfinite(x) for x in v.co)
        groups=[g for g in v.groups if g.weight>0]
        if len(groups)!=1 or abs(sum(g.weight for g in groups)-1)>1e-6:bad.append(v.index)
        for g in groups:
            name=obj.vertex_groups[g.group].name
            assert name in rig.data.bones and rig.data.bones[name].use_deform
            weighted[name]=weighted.get(name,0)+1
    assert not bad, (obj.name, bad[:10])
    assert len(weighted)==18
    assert obj.parent==rig
    assert len(obj.modifiers)==1 and obj.modifiers[0].object==rig
    assert obj.data.uv_layers.active.name=='UV0'
    uv=[d.uv for d in obj.data.uv_layers.active.data]
    assert all(-1e-5<=v.x<=1.00001 and -1e-5<=v.y<=1.00001 for v in uv)
    report['meshes'].append({'name':obj.name,'vertices':len(obj.data.vertices),'triangles':len(obj.data.loop_triangles),
        'weighted_bones':weighted,'unweighted_or_invalid_vertices':len(bad),'uv_layer':'UV0',
        'uv_within_0_1':True,'materials':[m.name for m in obj.data.materials]})

rest_positions={}
for frame in [1,25,50,75,100]:
    scene.frame_set(frame);bpy.context.view_layer.update()
    deps=bpy.context.evaluated_depsgraph_get()
    maxerr=0;maxmove=0
    for obj in meshes:
        evaluated=obj.evaluated_get(deps).to_mesh()
        matrices={g.index:rig.pose.bones[g.name].matrix @ rig.data.bones[g.name].matrix_local.inverted() for g in obj.vertex_groups}
        if frame==1:rest_positions[obj.name]=[v.co.copy() for v in evaluated.vertices]
        assert len(evaluated.vertices)==len(obj.data.vertices)
        for v,ev,rest in zip(obj.data.vertices,evaluated.vertices,rest_positions[obj.name]):
            pred=matrices[v.groups[0].group] @ v.co
            maxerr=max(maxerr,(pred-ev.co).length)
            maxmove=max(maxmove,(rest-ev.co).length)
        obj.evaluated_get(deps).to_mesh_clear()
    assert maxerr<2e-6,(frame,maxerr)
    if frame==50:assert maxmove>.05
    if frame==100:assert maxmove<2e-6
    report['pose_tests'].append({'frame':frame,'max_skinning_error_m':maxerr,'max_movement_from_rest_m':maxmove})

assert all(im.packed_file for im in bpy.data.images if im.source=='FILE' and im.type=='IMAGE')
report['packed_reference_images']=sum(1 for im in bpy.data.images if im.packed_file)
report['animation']=rig.animation_data.action.name
report['limitations']=['FK rig, not an Epic mannequin rig','Procedural Blender materials; no baked texture set',
    'Unreal import not tested','Auto UV unwrap; overlap and distortion not independently measured',
    'Demo poses visually inspected; arbitrary poses can require clearance adjustments']
scene.frame_set(1)
with open(os.path.join(OUT,'CyborgHands_Validation.json'),'w',encoding='utf-8') as f:json.dump(report,f,indent=2)
print(json.dumps({'result':'PASS','bone_count':report['bone_count'],'triangles':sum(m['triangles'] for m in report['meshes']),
    'vertices':sum(m['vertices'] for m in report['meshes']),'pose_tests':report['pose_tests']},indent=2),flush=True)
# A close view of a flexed hand from the palm side makes the articulation reviewable.
scene.frame_set(50)
cam=bpy.data.objects['CAM | Palms']
cam.location=(.78,2.5,1.1)
target=Vector((.38,.23,.91))
cam.rotation_euler=(target-cam.location).to_track_quat('-Z','Y').to_euler()
cam.data.ortho_scale=.36
scene.camera=cam
scene.render.resolution_x=1100;scene.render.resolution_y=1100
scene.render.filepath=os.path.join(OUT,'CyborgHands_GripDetail.png')
bpy.ops.render.render(write_still=True)
