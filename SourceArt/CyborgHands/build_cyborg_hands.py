"""Create an original reference-inspired, articulated hard-surface arm pair in Blender.
Run with Blender 5.0: --background --factory-startup --python this_file.py
"""
import bpy, bmesh, math, os, json
from mathutils import Vector, Matrix
from math import pi, sin, cos

OUT = os.path.dirname(os.path.abspath(__file__))
BLEND = os.path.join(OUT, 'CyborgHands.blend')
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
for block in list(bpy.data.collections):
    if block.name != 'Collection':
        bpy.data.collections.remove(block)
scene = bpy.context.scene
scene.unit_settings.system = 'METRIC'
scene.unit_settings.scale_length = 1.0
base = bpy.data.collections.get('Collection')
base.name = '01 | CYBORG HANDS'
rigcol = bpy.data.collections.new('02 | SKELETON - FK')
scene.collection.children.link(rigcol)
stage = bpy.data.collections.new('03 | STUDIO - exclude from export')
scene.collection.children.link(stage)
refs = bpy.data.collections.new('04 | REFERENCES - hidden')
scene.collection.children.link(refs)
parts = []
bones = []

def relocate(obj, col=base):
    for c in list(obj.users_collection): c.objects.unlink(obj)
    col.objects.link(obj)

def material(name, color, metallic, rough):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    p = mat.node_tree.nodes.get('Principled BSDF')
    p.inputs['Base Color'].default_value = (*color, 1)
    p.inputs['Metallic'].default_value = metallic
    p.inputs['Roughness'].default_value = rough
    mat.diffuse_color = (*color, 1)
    return mat

armor = material('01 | Obsidian - ceramic titanium', (.021, .026, .033), .65, .36)
titanium = material('02 | Gunmetal - machined edges', (.080, .094, .112), .82, .35)
edge = material('03 | Satin titanium - fasteners', (.22, .245, .275), .88, .30)
dark = material('04 | Elastomer - seals and recesses', (.009, .012, .015), .12, .54)
carbon = material('05 | Woven carbon - flexible underlayer', (.024, .029, .033), .35, .43)
orange = material('06 | Amber - illuminated power strips', (.42, .075, .012), .45, .26)
p = orange.node_tree.nodes.get('Principled BSDF')
p.inputs['Emission Color'].default_value = (1, .245, .035, 1)
p.inputs['Emission Strength'].default_value = 5
marking = material('07 | Warm silver - etched identification', (.59, .61, .59), .6, .41)

# Subtle procedural machining and a small carbon weave. All materials are self-contained.
for mat in [armor, titanium]:
    n, l = mat.node_tree.nodes, mat.node_tree.links
    tex = n.new('ShaderNodeTexNoise'); tex.inputs['Scale'].default_value = 185
    tex.inputs['Detail'].default_value = 2
    ramp = n.new('ShaderNodeValToRGB')
    ramp.color_ramp.elements[0].color = (.31, .31, .31, 1)
    ramp.color_ramp.elements[1].color = (.43, .43, .43, 1)
    l.new(tex.outputs['Fac'], ramp.inputs[0])
    l.new(ramp.outputs['Color'], n.get('Principled BSDF').inputs['Roughness'])
    bump = n.new('ShaderNodeBump'); bump.inputs['Strength'].default_value = .13
    bump.inputs['Distance'].default_value = .00008
    l.new(tex.outputs['Fac'], bump.inputs['Height'])
    l.new(bump.outputs['Normal'], n.get('Principled BSDF').inputs['Normal'])
n, l = carbon.node_tree.nodes, carbon.node_tree.links
tc = n.new('ShaderNodeTexCoord')
mapping = n.new('ShaderNodeVectorMath'); mapping.operation = 'SCALE'
mapping.inputs[3].default_value = 550
l.new(tc.outputs['Object'], mapping.inputs[0])
checker = n.new('ShaderNodeTexChecker'); checker.inputs['Scale'].default_value = 1
checker.inputs['Color1'].default_value = (.009, .014, .017, 1)
checker.inputs['Color2'].default_value = (.035, .043, .049, 1)
l.new(mapping.outputs[0], checker.inputs['Vector'])
l.new(checker.outputs['Color'], n.get('Principled BSDF').inputs['Base Color'])
bump = n.new('ShaderNodeBump'); bump.inputs['Strength'].default_value = .26
bump.inputs['Distance'].default_value = .00015
l.new(checker.outputs['Fac'], bump.inputs['Height'])
l.new(bump.outputs[0], n.get('Principled BSDF').inputs['Normal'])

class Frame:
    def __init__(self, start, end):
        self.p = Vector(start)
        self.d = (Vector(end)-self.p).normalized()
        self.u = self.d.cross(Vector((0, 1, 0))).normalized()
        self.v = self.d.cross(self.u).normalized()
        self.length = (Vector(end)-self.p).length
    def at(self, u=0, v=0, t=0): return self.p + self.u*u + self.v*v + self.d*t

def mesh(name, verts, faces, mat, bone, bevel=0, smooth=False):
    data = bpy.data.meshes.new(name)
    data.from_pydata(verts, [], faces); data.update()
    bm = bmesh.new(); bm.from_mesh(data)
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    bm.to_mesh(data); bm.free()
    obj = bpy.data.objects.new(name, data); base.objects.link(obj)
    obj.data.materials.append(mat)
    for poly in data.polygons: poly.use_smooth = smooth
    if bevel:
        m = obj.modifiers.new('Precision edge radii', 'BEVEL')
        m.width = bevel; m.segments = 2
        m = obj.modifiers.new('Face weighted normals', 'WEIGHTED_NORMAL')
        m.keep_sharp = True; m.weight = 40
    if bone:
        obj.vertex_groups.new(name=bone).add(list(range(len(data.vertices))), 1.0, 'REPLACE')
        obj['Rigid mechanical segment'] = bone
        parts.append(obj)
    return obj

def tube(name, f, profiles, mat, bone, segments=32, bevel=.001, smooth=True):
    vs = [f.at(ru*cos(2*pi*i/segments), rv*sin(2*pi*i/segments), t)
          for t,ru,rv in profiles for i in range(segments)]
    fs = []
    for j in range(len(profiles)-1):
        for i in range(segments):
            k = j*segments+i; k1 = j*segments+(i+1)%segments
            fs.append((k,k1,k1+segments,k+segments))
    fs += [tuple(range(segments-1,-1,-1)), tuple((len(profiles)-1)*segments+i for i in range(segments))]
    return mesh(name, vs, fs, mat, bone, bevel, smooth)

def shell(name, f, rings, amin, amax, mat, bone, thick=.003, steps=12, bevel=.0012, skew=0):
    """Closed armor panel: each ring is t, radius U, radius V, angular inset degrees."""
    vs=[]
    for inner in [False, True]:
        for t,ru,rv,inset in rings:
            for i in range(steps+1):
                a = math.radians(amin+inset + (amax-amin-2*inset)*i/steps)
                r = thick if inner else 0
                vs.append(f.at((ru-r)*cos(a), (rv-r)*sin(a), t+skew*cos(a)))
    m=steps+1; count=len(rings)*m; fs=[]
    for layer in range(2):
        k=layer*count
        for j in range(len(rings)-1):
            for i in range(steps):
                a=k+j*m+i; fs.append((a,a+1,a+m+1,a+m))
    for j in [0,len(rings)-1]:
        for i in range(steps):
            a=j*m+i; fs.append((a,a+1,a+1+count,a+count))
    for i in [0,steps]:
        for j in range(len(rings)-1):
            a=j*m+i; fs.append((a,a+m,a+m+count,a+count))
    return mesh(name,vs,fs,mat,bone,bevel,True)

def prism(name, f, polygon, v, depth, mat, bone, bevel=.001):
    # Polygon coordinates are (lateral, longitudinal).
    vs=[f.at(u,vv,t) for vv in [v-depth,v] for u,t in polygon]
    m=len(polygon)
    fs=[tuple(range(m-1,-1,-1)),tuple(range(m,2*m))]
    fs += [(i,(i+1)%m,(i+1)%m+m,i+m) for i in range(m)]
    return mesh(name,vs,fs,mat,bone,bevel)

def rod(name,a,b,radius,mat,bone,radius2=None,segments=16):
    f=Frame(a,b)
    return tube(name,f,[(0,radius,radius),(f.length,radius2 or radius,radius2 or radius)],mat,bone,segments,min(radius*.18,.0007))

def line(name, points, radius, mat, bone):
    for i in range(len(points)-1):
        rod(name+str(i),points[i],points[i+1],radius,mat,bone,segments=8)

def ball(name, at, scale, mat, bone, seg=20, rings=12):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=seg, ring_count=rings, location=at)
    obj=bpy.context.object; obj.name=name; relocate(obj)
    obj.scale=scale
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    obj.data.materials.append(mat)
    for p in obj.data.polygons:p.use_smooth=True
    obj.vertex_groups.new(name=bone).add(list(range(len(obj.data.vertices))),1,'REPLACE')
    parts.append(obj)
    return obj

def screw(name, point, normal, bone, radius=.0032):
    p=Vector(point); n=Vector(normal).normalized()
    rod(name+' | countersink',p-n*.0006,p+n*.0007,radius*1.3,dark,bone,segments=16)
    rod(name+' | captive bolt',p+n*.0006,p+n*.0015,radius,edge,bone,segments=12)
    rod(name+' | hex socket',p+n*.0015,p+n*.00165,radius*.48,dark,bone,segments=6)

def addbone(name,head,tail,parent,connected=False):
    bones.append(dict(name=name,head=Vector(head),tail=Vector(tail),parent=parent,connected=connected))

# Model the right side in world coordinates, then mirror geometry and bone definitions.
side='r'
shoulder=Vector((.245,0,1.48)); elbow=Vector((.295,0,1.185)); wrist=Vector((.352,0,.915))
handend=wrist+Vector((.015,0,-.107))
upper=Frame(shoulder,elbow); lower=Frame(elbow,wrist); hand=Frame(wrist,handend)
addbone('clavicle_r',(.12,0,1.48),shoulder,'root')
addbone('upperarm_r',shoulder,elbow,'clavicle_r',True)
addbone('lowerarm_r',elbow,wrist,'upperarm_r',True)
addbone('hand_r',wrist,handend,'lowerarm_r',True)

# Shoulder: layered pauldron, floating deltoid guards and a visible mechanical socket.
bn='upperarm_r'; L=upper.length
ball('Shoulder | dark socket',shoulder,(.064,.054,.058),dark,bn,32,20)
tube('Upper arm | carbon body',upper,[(.015,.060,.049),(.065,.057,.047),(.15,.049,.040),(L-.03,.036,.032)],carbon,bn)
shell('Shoulder | titanium edge',upper,[(-.027,.061,.058,24),(0,.079,.066,7),(.029,.081,.067,0),(.086,.064,.054,15)],-19,200,titanium,bn,.002,20)
shell('Shoulder | ceramic cap',upper,[(-.027,.063,.060,24),(0,.081,.068,7),(.029,.083,.069,0),(.086,.066,.056,15)],-13,194,armor,bn,.004,20)
shell('Shoulder | inset dorsal panel',upper,[(-.010,.078,.071,10),(.024,.087,.074,0),(.060,.079,.067,5)],49,133,dark,bn,.0018,12,.0006)
shell('Shoulder | floating outer guard',upper,[(.058,.073,.056,10),(.09,.069,.053,0),(.136,.058,.047,15)],-38,43,armor,bn,.004,10)
shell('Biceps | outer shell',upper,[(.085,.060,.049,12),(.13,.059,.049,0),(.205,.047,.040,2),(L-.037,.041,.035,17)],-28,48,armor,bn,.004,10)
shell('Biceps | inner shell',upper,[(.095,.057,.047,15),(.155,.053,.045,0),(L-.039,.040,.035,10)],137,213,armor,bn,.004,10)
shell('Biceps | dorsal carbon window',upper,[(.085,.059,.051,8),(.14,.054,.047,0),(.23,.040,.036,10)],53,128,carbon,bn,.002,12,.0005)
shell('Triceps | armor',upper,[(.08,.058,.049,10),(.15,.054,.045,0),(.248,.039,.035,10)],224,316,armor,bn,.004,12)
for a in [45,136]:
    angle=math.radians(a)
    line('Biceps | titanium seam', [upper.at(.061*cos(angle),.051*sin(angle),.09),upper.at(.057*cos(angle),.049*sin(angle),.14),upper.at(.042*cos(angle),.037*sin(angle),.234)],.0010,titanium,bn)
for t in [.071,.108]:
    screw('Shoulder | service fastener',upper.at(.050,.050,t),upper.v,bn,.0038)
for i in range(3):
    t=.204+i*.010
    prism('Upper | light gasket',upper,[(-.005,t),(.005,t),(.006,t+.004),(-.006,t+.004)],.039,.003,dark,bn,.001)
    prism('Upper | amber status',upper,[(-.0025,t+.0005),(.0025,t+.0005),(.0025,t+.0032),(-.0025,t+.0032)],.040,.0015,orange,bn,.0006)

# Elbow: concentric hinge, side axle and a separate articulated cap.
bn='lowerarm_r'; L=lower.length
ball('Elbow | elastomer gaiter',elbow,(.037,.034,.038),dark,bn,28,16)
rod('Elbow | axle',elbow-lower.u*.045,elbow+lower.u*.045,.022,titanium,bn,segments=32)
for s in [-1,1]:
    p=elbow+lower.u*.045*s
    rod('Elbow | knurled hub',p-lower.u*.004*s,p+lower.u*.003*s,.024,armor,bn,segments=32)
    rod('Elbow | bearing',p+lower.u*.003*s,p+lower.u*.004*s,.016,edge,bn,segments=32)
    rod('Elbow | recessed end',p+lower.u*.004*s,p+lower.u*.0045*s,.011,dark,bn,segments=24)
    screw('Elbow | axial bolt',p+lower.u*.0046*s,lower.u*s,bn,.005)
shell('Elbow | dorsal shield',lower,[(-.022,.034,.038,16),(0,.041,.042,0),(.027,.043,.041,12)],35,145,armor,bn,.004,14)
for i in range(5):
    t=.012+i*.007
    tube('Elbow | bellows',lower,[(t,.037,.033),(t+.0025,.038,.034),(t+.005,.037,.033)],dark,bn,32,.0004)

# Forearm: elongated, tapered overlapping armor with narrow orange tracer lines.
tube('Forearm | carbon core',lower,[(.035,.039,.032),(.08,.044,.037),(.165,.036,.031),(L-.018,.025,.023)],carbon,bn,32,.001)
shell('Forearm | dorsal underframe',lower,[(.044,.046,.041,16),(.077,.054,.047,0),(.141,.046,.044,0),(.162,.044,.041,0),(.167,.0435,.0405,0),(.214,.036,.033,5),(L-.034,.032,.030,13)],22,160,titanium,bn,.002,18,skew=.012)
shell('Forearm | proximal ceramic bracer',lower,[(.044,.048,.043,16),(.077,.056,.049,0),(.141,.048,.046,0),(.162,.046,.043,0)],26,156,armor,bn,.0045,18,skew=.012)
shell('Forearm | distal ceramic bracer',lower,[(.167,.0455,.0425,0),(.214,.038,.035,5),(L-.034,.034,.032,13)],26,156,armor,bn,.0045,18,skew=.012)
shell('Forearm | dorsal inset proximal',lower,[(.075,.055,.050,8),(.098,.053,.052,0),(.153,.046,.046,0)],70,110,armor,bn,.0025,8,skew=.012)
shell('Forearm | dorsal inset distal',lower,[(.172,.044,.0435,0),(.215,.038,.037,2),(.236,.034,.033,12)],70,110,armor,bn,.0025,8,skew=.012)
shell('Forearm | outer floating blade',lower,[(.06,.056,.039,8),(.089,.056,.042,0),(.186,.043,.037,0),(.239,.035,.027,9)],-35,22,armor,bn,.004,8)
shell('Forearm | inner plate',lower,[(.065,.052,.040,11),(.105,.051,.041,0),(.225,.033,.029,9)],158,211,armor,bn,.0038,8)
shell('Forearm | palmar plate',lower,[(.071,.050,.041,8),(.11,.050,.043,0),(.21,.036,.030,3),(.245,.031,.027,11)],225,315,armor,bn,.0038,12)
for a in [29,153]:
    th=math.radians(a)
    line('Forearm | edge pinstripe',[lower.at(.054*cos(th),.048*sin(th),.088),lower.at(.046*cos(th),.044*sin(th),.156),lower.at(.034*cos(th),.032*sin(th),.236)],.00085,edge,bn)
# Diagonal dorsal light breaks, with dark inset bezels.
for sign,t in [(1,.122),(-1,.190)]:
    a=lower.at(sign*.010,.052 if t<.15 else .043,t)
    b=lower.at(sign*.024,.049 if t<.15 else .040,t+.017)
    rod('Forearm | recessed tracer channel',a,b,.0026,dark,bn,segments=12)
    rod('Forearm | amber tracer',a+lower.v*.0023,b+lower.v*.0023,.00135,orange,bn,segments=12)
# Three recessed slots break up the outside edge without changing the silhouette.
for i in range(3):
    t=.087+i*.010
    a=lower.at(.045,.029,t); b=lower.at(.049,.019,t+.006)
    rod('Forearm | cooling channel',a,b,.0021,dark,bn,segments=12)
    rod('Forearm | vent bottom',a+lower.u*.0005,b+lower.u*.0005,.00065,titanium,bn,segments=8)
for t,ru,rv in [(.075,.049,.045),(.223,.030,.029)]:
    for sign in [-1,1]:
        screw('Forearm | captive fastener',lower.at(sign*ru*.70,rv*.73,t),lower.v,bn,.003)
# Vented strips and exposed paired hydraulic lines on palm side.
for i in range(5):
    t=.085+i*.009
    prism('Forearm | heat-sink fin',lower,[(-.018,t),(.018,t),(.018,t+.004),(-.018,t+.004)],-.043,.0018,titanium,bn,.0004)
for s in [-1,1]:
    a=lower.at(s*.026,-.030,.151); b=lower.at(s*.021,-.022,.228)
    rod('Forearm | hydraulic housing',a,b,.0042,dark,bn)
    rod('Forearm | piston rod',b,lower.at(s*.019,-.019,.26),.0022,edge,bn)

# Wrist collar and three gimbal layers; hand geometry is attached to hand_r.
tube('Wrist | proximal cuff',lower,[(L-.026,.030,.027),(L-.023,.034,.030),(L-.009,.033,.030),(L-.006,.028,.026)],titanium,'lowerarm_r',32,.001)
bn='hand_r'
tube('Wrist | flexible seal',hand,[(-.006,.027,.025),(.007,.028,.025),(.014,.031,.026)],dark,bn,32,.001)
tube('Wrist | distal ring',hand,[(.001,.030,.027),(.007,.032,.028),(.013,.031,.027)],armor,bn,32,.0008)
for s in [-1,1]:
    screw('Wrist | pivot',hand.at(s*.032,0,.006),hand.u*s,bn,.005)

# Palm: solid carbon housing with stepped dorsal shields and armored knuckle bar.
tube('Palm | composite body',hand,[(.012,.029,.021),(.028,.039,.024),(.070,.043,.023),(.099,.038,.020),(.108,.034,.016)],carbon,bn,32,.0015)
outline=[(-.026,.014),(.026,.014),(.036,.028),(.041,.075),(.031,.101),(-.031,.101),(-.041,.075),(-.036,.028)]
prism('Hand | titanium perimeter',hand,outline,.025,.006,titanium,bn,.0016)
outline2=[(-.024,.017),(.024,.017),(.034,.030),(.038,.072),(.029,.096),(-.029,.096),(-.038,.072),(-.034,.030)]
prism('Hand | dorsal shield',hand,outline2,.028,.0045,armor,bn,.0018)
prism('Hand | central floating plate',hand,[(-.021,.029),(.019,.025),(.027,.062),(.011,.085),(-.025,.073)],.0302,.003,armor,bn,.001)
line('Hand | engraved seam',[hand.at(-.020,.031,.030),hand.at(-.024,.031,.072),hand.at(.010,.031,.084),hand.at(.027,.031,.062)],.0006,titanium,bn)
prism('Palm | heel pad',hand,[(-.025,.021),(.023,.021),(.030,.055),(.018,.077),(-.022,.069)],-.024,.004,dark,bn,.003)
for i in range(3):
    t=.070+i*.008
    prism('Palm | grip rib',hand,[(-.029,t),(.026,t),(.027,t+.0035),(-.029,t+.0035)],-.022,.0035,dark,bn,.001)
for u,t in [(-.026,.028),(.026,.028),(-.027,.077),(.027,.077)]:
    screw('Hand | recessed screw',hand.at(u,.029,t),hand.v,bn,.0025)
prism('Hand | status recess',hand,[(-.010,.018),(.010,.018),(.010,.0225),(-.010,.0225)],.029,.002,dark,bn,.0007)
prism('Hand | amber status',hand,[(-.006,.0188),(.006,.0188),(.006,.0214),(-.006,.0214)],.030,.001,orange,bn,.0005)

# Anatomical finger spacing and a three-bone opposable thumb.
digits=[('index',-.027,.096,[.037,.026,.022],.0095,-.10),
        ('middle',-.0085,.106,[.042,.029,.022],.0100,-.015),
        ('ring',.012,.103,[.039,.026,.020],.0093,.04),
        ('pinky',.030,.094,[.030,.021,.018],.0082,.14)]
finger_heads={}
for digit,u,t,lengths,r,spread in digits:
    start=hand.at(u,0,t)
    direction=(hand.d+hand.u*spread).normalized()
    points=[start]
    for length in lengths: points.append(points[-1]+direction*length)
    finger_heads[digit]=points
    for j in range(3):
        bn=f'{digit}_{j+1:02d}_r'
        parent='hand_r' if j==0 else f'{digit}_{j:02d}_r'
        addbone(bn,points[j],points[j+1],parent,j>0)
        f=Frame(points[j],points[j+1]); ln=f.length; rr=r*(1-.11*j)
        ball(f'{digit} {j+1} | knuckle',points[j],(rr*.94,rr*.87,rr*.94),dark,bn,20,12)
        tube(f'{digit} {j+1} | core',f,[(.002,rr*.83,rr*.75),(ln-.004,rr*.73,rr*.68)],dark,bn,16,.00065)
        shell(f'{digit} {j+1} | phalanx armor',f,[(.004,rr,rr*.96,9),(.008,rr*1.02,rr*.98,0),(ln-.005,rr*.85,rr*.80,7)],4,176,armor,bn,.0016,10,.00065)
        shell(f'{digit} {j+1} | knuckle bridge',f,[(.001,rr*1.03,rr*.94,14),(.004,rr*1.05,rr,0),(.009,rr*.99,rr*.94,12)],30,150,titanium,bn,.0014,10,.0005)
        shell(f'{digit} {j+1} | grip pad',f,[(.007,rr*.89,rr*.83,7),(ln-.006,rr*.78,rr*.77,9)],204,336,dark,bn,.0017,10,.0005)
        for s in [-1,1]:
            p=f.at(s*rr*.92,0,.002)
            rod(f'{digit} | bearing',p,p+f.u*.001*s,rr*.62,titanium,bn,segments=16)
            screw(f'{digit} | hinge',p+f.u*.001*s,f.u*s,bn,rr*.27)
        if j==2:
            ball(f'{digit} | fingertip',f.at(0,0,ln-.003),(rr*.78,rr*.72,rr*.6),dark,bn,16,10)
        for k in range(2):
            tt=.011+k*.004
            if tt<ln-.007:
                line(f'{digit} | tactile groove',[f.at(-rr*.62,-rr*.79,tt),f.at(rr*.62,-rr*.79,tt)],.00055,titanium,bn)

thumbpts=[hand.at(-.030,-.008,.031),hand.at(-.054,-.014,.056),hand.at(-.071,-.012,.086),hand.at(-.079,-.006,.116)]
for j in range(3):
    bn=f'thumb_{j+1:02d}_r'
    addbone(bn,thumbpts[j],thumbpts[j+1],'hand_r' if j==0 else f'thumb_{j:02d}_r',j>0)
    f=Frame(thumbpts[j],thumbpts[j+1]); ln=f.length; rr=[.014,.011,.0095][j]
    ball('Thumb | knuckle',thumbpts[j],(rr,rr*.9,rr),dark,bn,24,14)
    tube('Thumb | flexible core',f,[(.002,rr*.90,rr*.80),(ln-.003,rr*.78,rr*.73)],dark,bn,20,.0007)
    shell('Thumb | independent armor',f,[(.003,rr*1.04,rr,12),(.01,rr*1.05,rr,0),(ln-.004,rr*.86,rr*.86,8)],5,177,armor,bn,.0018,12,.0008)
    shell('Thumb | silver knuckle',f,[(.001,rr*1.04,rr,14),(.007,rr*1.05,rr,2)],26,151,titanium,bn,.0015,12,.0006)
    shell('Thumb | grip surface',f,[(.008,rr*.95,rr*.93,12),(ln-.004,rr*.79,rr*.82,9)],205,332,dark,bn,.002,12,.0005)
    for s in [-1,1]: screw('Thumb | hinge',f.at(s*rr,0,.004),f.u*s,bn,rr*.30)

print('RIGHT ARM CREATED:',len(parts),'components',flush=True)

# Bake bevels and normals into each evaluated mesh before joining. Mechanical parts
# keep rigid weights, so metal plates do not deform like rubber when a joint bends.
deps=bpy.context.evaluated_depsgraph_get()
for obj in parts:
    bind_bone=obj.vertex_groups[0].name
    if obj.modifiers:
        evaluated=obj.evaluated_get(deps)
        new=bpy.data.meshes.new_from_object(evaluated,preserve_all_data_layers=True,depsgraph=deps)
        old=obj.data; obj.modifiers.clear(); obj.data=new
        if old.users==0:bpy.data.meshes.remove(old)
    # Bevel can create vertices with empty deform data on intersecting corners.
    # Each source object is a single rigid part, so rebinding every final vertex
    # to its intended bone is exact and does not depend on interpolation.
    obj.vertex_groups.clear()
    obj.vertex_groups.new(name=bind_bone).add(list(range(len(obj.data.vertices))),1.0,'REPLACE')
bpy.ops.object.select_all(action='DESELECT')
for obj in parts: obj.select_set(True)
bpy.context.view_layer.objects.active=parts[0]
bpy.ops.object.join()
right=bpy.context.object; right.name='CyborgHands_R | articulated armor'
bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
# Stable UV maps for future painting. Current appearance uses procedural materials.
bpy.ops.object.mode_set(mode='EDIT'); bpy.ops.mesh.select_all(action='SELECT')
bpy.ops.uv.smart_project(angle_limit=math.radians(66),island_margin=.003,correct_aspect=True)
bpy.ops.object.mode_set(mode='OBJECT')
right.data.uv_layers.active.name='UV0'

left=right.copy();left.data=right.data.copy();base.objects.link(left)
left.name='CyborgHands_L | articulated armor'
for v in left.data.vertices:v.co.x=-v.co.x
bm=bmesh.new();bm.from_mesh(left.data)
bmesh.ops.reverse_faces(bm,faces=list(bm.faces))
bm.to_mesh(left.data);bm.free()
for vg in left.vertex_groups: vg.name=vg.name[:-1]+'l'
right['Description']='Right arm, shoulder to fingertip. Rigid weights per mechanical segment.'
left['Description']='Mirrored left arm. Distinct left-side bone groups.'

# Build the skeleton in Blender space, with meter units and separate left/right chains.
armdata=bpy.data.armatures.new('CyborgHands_Skeleton')
rig=bpy.data.objects.new('CyborgHands_Rig',armdata);rigcol.objects.link(rig)
bpy.ops.object.select_all(action='DESELECT');rig.select_set(True);bpy.context.view_layer.objects.active=rig
bpy.ops.object.mode_set(mode='EDIT')
root=armdata.edit_bones.new('root');root.head=(0,0,1.36);root.tail=(0,0,1.47)
for spec in list(bones):
    mirror=dict(spec)
    mirror['name']=spec['name'][:-1]+'l'
    mirror['head']=Vector((-spec['head'].x,spec['head'].y,spec['head'].z))
    mirror['tail']=Vector((-spec['tail'].x,spec['tail'].y,spec['tail'].z))
    mirror['parent']=spec['parent'][:-1]+'l' if spec['parent']!='root' else 'root'
    bones.append(mirror)
for spec in bones:
    eb=armdata.edit_bones.new(spec['name']);eb.head=spec['head'];eb.tail=spec['tail']
    eb.parent=armdata.edit_bones[spec['parent']]
    eb.use_connect=spec['connected']
    eb.align_roll(Vector((0,-1,0)))
bpy.ops.object.mode_set(mode='OBJECT')
armdata.bones['root'].use_deform=False
for s in ['r','l']:
    coll=armdata.collections.new('Right arm' if s=='r' else 'Left arm')
    for bone in armdata.bones:
        if bone.name.endswith('_'+s):coll.assign(bone)
        if bone.name.startswith('clavicle'):bone.use_deform=False
        bone.color.palette='THEME04' if bone.name.endswith('_l') else 'THEME03'
rig.show_in_front=True;armdata.display_type='OCTAHEDRAL'
rig['Rig type']='FK. Pose upperarm / lowerarm / hand and 3 bones per finger. Rest pose at frame 1.'
rig['Scale']='Metric, meters. Approximately 0.87 m shoulder-to-fingertip.'
rig['Demo']='Action CyborgHands_Grip_Test: frames 1 and 100 open; frame 50 closed, elbows flexed.'
for obj in [right,left]:
    obj.parent=rig
    mod=obj.modifiers.new('CyborgHands | rigid skeletal binding','ARMATURE');mod.object=rig
    mod.use_vertex_groups=True
    obj['Export note']='Select both arm meshes and CyborgHands_Rig; exclude the STUDIO collection.'
for p in rig.pose.bones:p.rotation_mode='XYZ'
for frame,amount in [(1,0),(25,.38),(50,1),(75,.38),(100,0)]:
    for s in ['r','l']:
        for digit in ['index','middle','ring','pinky','thumb']:
            angles=[18,40,45] if digit=='thumb' else [68,88,65]
            for j,a in enumerate(angles):
                pb=rig.pose.bones[f'{digit}_{j+1:02d}_{s}']
                pb.rotation_euler.x=-math.radians(a)*amount
                if digit=='thumb' and j==0:
                    pb.rotation_euler.z=math.radians(55)*amount*(1 if s=='r' else -1)
                pb.keyframe_insert(data_path='rotation_euler',frame=frame,group=f'{s.upper()} | {digit}')
        pb=rig.pose.bones['lowerarm_'+s];pb.rotation_euler.x=-math.radians(32)*amount
        pb.keyframe_insert(data_path='rotation_euler',frame=frame,group=s.upper()+' | arm')
        pb=rig.pose.bones['hand_'+s];pb.rotation_euler.x=-math.radians(9)*amount
        pb.keyframe_insert(data_path='rotation_euler',frame=frame,group=s.upper()+' | arm')
rig.animation_data.action.name='CyborgHands_Grip_Test'
scene.frame_start=1;scene.frame_end=100;scene.render.fps=30;scene.frame_set(1)
for frame,name in [(1,'OPEN - REST'),(50,'GRIP + ELBOW FLEX'),(100,'OPEN')]:scene.timeline_markers.new(name,frame=frame)

# Pack the supplied images for convenient reference without displaying them over the model.
for i,path in enumerate([
 r'C:\Users\Vadim\AppData\Local\Temp\codex-clipboard-e35c4394-1743-468d-86fc-f8bd36e39ae4.png',
 r'C:\Users\Vadim\AppData\Local\Temp\codex-clipboard-93098802-8114-4b86-949c-b3911905810c.png']):
    if os.path.exists(path):
        img=bpy.data.images.load(path);img.pack()
        obj=bpy.data.objects.new('Reference '+str(i+1),None);refs.objects.link(obj)
        obj.empty_display_type='IMAGE';obj.data=img;obj.location=(1.5+i,1,1.2)
        obj.empty_display_size=.8;obj.hide_render=True
refs.hide_viewport=True;refs.hide_render=True

readme=bpy.data.texts.new('README - CyborgHands')
readme.write('''CYBORG HANDS / original reference-inspired model

PAIR: left and right arms, shoulders, upper arms, elbows, forearms,
wrists, palms and all fingers. Dark titanium, carbon surfaces, amber lights.
SCALE: meters. Blender Z up.

RIG: CyborgHands_Rig. FK skeleton, separate left/right chains, three bones
per digit. Rigid 1.0 skin weights keep mechanical armor pieces solid.
Select the armature, enter Pose Mode, rotate joints.

DEMO: CyborgHands_Grip_Test, frames 1-100. Frame 1 = rest/open.
Frame 50 = grip + elbow flex. The animation is a rig test, not a polished
gameplay animation. Clear/mute the action before authoring a new animation.

MATERIALS: self-contained procedural Blender shaders. UV0 is automatically
unwrapped; texture maps have not been baked. Unreal import and retargeting
have not been tested. This is not an Epic mannequin-compatible skeleton.

EXPORT: select CyborgHands_Rig and the two meshes in collection 01.
Collection 03 contains only preview lights, cameras and the display stage.
Collection 04 holds packed source reference images, hidden by default.
''')

# Studio: broad highlights reveal the layered armor without brightening black metal.
floor=material('STUDIO | charcoal',(.013,.018,.026),.25,.46)
podium=material('STUDIO | pedestal',(.025,.032,.044),.65,.31)
bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,.555))
obj=bpy.context.object;obj.name='Studio floor';relocate(obj,stage);obj.data.materials.append(floor)
for s in [-1,1]:
    bpy.ops.mesh.primitive_cylinder_add(vertices=96,radius=.18,depth=.027,location=(s*.354,0,.57))
    obj=bpy.context.object;obj.name='Display plinth';relocate(obj,stage);obj.data.materials.append(podium)
    mod=obj.modifiers.new('Soft perimeter','BEVEL');mod.width=.004;mod.segments=3
    obj.modifiers.new('Normals','WEIGHTED_NORMAL')

def camera(name,loc,target,ortho):
    data=bpy.data.cameras.new(name);obj=bpy.data.objects.new(name,data);stage.objects.link(obj)
    obj.location=loc;obj.rotation_euler=(Vector(target)-obj.location).to_track_quat('-Z','Y').to_euler()
    data.type='ORTHO';data.ortho_scale=ortho;data.lens=70
    return obj

hero=camera('CAM | Hero - dorsal armor',(.78,-3.7,1.59),(0,0,1.08),1.18)
palmcam=camera('CAM | Palms',(-.8,3.7,1.40),(0,0,1.08),1.19)
detail=camera('CAM | Hands detail',(.64,-2.8,1.02),(0,0,.84),.92)
rigcam=camera('CAM | Rig test',(.9,-3.4,1.56),(0,0,1.1),1.23)
scene.camera=hero

def area(name,loc,target,power,color,size,sizey=None):
    data=bpy.data.lights.new(name,'AREA');obj=bpy.data.objects.new(name,data);stage.objects.link(obj)
    obj.location=loc;obj.rotation_euler=(Vector(target)-obj.location).to_track_quat('-Z','Y').to_euler()
    data.energy=power;data.color=color
    if sizey:data.shape='RECTANGLE';data.size=size;data.size_y=sizey
    else:data.shape='DISK';data.size=size

area('Key | tall softbox',(-1.4,-2.0,2.4),(0,0,1.1),180,(.83,.91,1),1.1,2.4)
area('Fill | warm softbox',(1.6,-.6,1.6),(0,0,1.0),105,(1,.83,.66),.8,1.8)
area('Rim | ice',(.7,1.1,2.0),(0,0,1.15),240,(.58,.76,1),.75,1.7)
area('Rim | left',(-1.3,.4,1.25),(0,0,1.0),115,(1,.51,.23),.55,1.5)
area('Top | strip',(0,0,2.5),(0,0,1),95,(1,1,1),.7,1.5)
scene.world.color=(.12,.12,.12)
scene.world.use_nodes=True
scene.world.node_tree.nodes['Background'].inputs[0].default_value=(.12,.16,.23,1)
scene.world.node_tree.nodes['Background'].inputs[1].default_value=.3
scene.render.engine='CYCLES';scene.cycles.samples=48;scene.cycles.use_denoising=True
try:
    pref=bpy.context.preferences.addons['cycles'].preferences
    pref.compute_device_type='OPTIX';pref.get_devices()
    has_gpu=False
    for dev in pref.devices:
        dev.use=(dev.type!='CPU');has_gpu=has_gpu or dev.use
        print('RENDER DEVICE:',dev.name,dev.type,dev.use,flush=True)
    if has_gpu:scene.cycles.device='GPU'
except Exception as exc: print('CPU RENDER:',exc,flush=True)
scene.render.resolution_x=1600;scene.render.resolution_y=1600;scene.render.resolution_percentage=100
scene.render.image_settings.file_format='PNG';scene.render.image_settings.color_mode='RGBA'
scene.render.film_transparent=False
scene.view_settings.view_transform='AgX'
scene.view_settings.look='AgX - Medium High Contrast'
scene.render.filepath=os.path.join(OUT,'CyborgHands_Preview.png')
# Set a useful modeling view in the saved file.
for screen in bpy.data.screens:
    for area_ui in screen.areas:
        if area_ui.type=='VIEW_3D':
            space=area_ui.spaces.active
            space.region_3d.view_distance=1.5
            space.region_3d.view_location=(0,0,1.08)
            space.region_3d.view_rotation=hero.rotation_euler.to_quaternion()
            space.shading.type='MATERIAL'
            space.overlay.show_floor=False;space.overlay.show_axis_x=False;space.overlay.show_axis_y=False
            space.clip_end=100
scene['Asset']='CyborgHands | articulated cybernetic arm pair'
scene['Notes']='Reference-inspired original. Procedural materials; see embedded README.'
bpy.ops.object.select_all(action='DESELECT');rig.select_set(True);bpy.context.view_layer.objects.active=rig
# Keep studio helpers out of the modeling viewport, but available to the renderer.
for obj in stage.objects:
    if obj.type in {'LIGHT','CAMERA'}:obj.hide_set(True)
bpy.ops.wm.save_as_mainfile(filepath=BLEND)
print('SAVED',BLEND,flush=True)
bpy.ops.render.render(write_still=True)
print('HERO_RENDER_DONE',flush=True)
scene.camera=detail;scene.render.filepath=os.path.join(OUT,'CyborgHands_Detail.png')
scene.render.resolution_x=1800;scene.render.resolution_y=1200
bpy.ops.render.render(write_still=True)
print('DETAIL_RENDER_DONE',flush=True)
scene.camera=palmcam;scene.render.filepath=os.path.join(OUT,'CyborgHands_Palms.png')
scene.render.resolution_x=1400;scene.render.resolution_y=1400
bpy.ops.render.render(write_still=True)
print('PALMS_RENDER_DONE',flush=True)
scene.frame_set(50);scene.camera=rigcam
scene.render.filepath=os.path.join(OUT,'CyborgHands_GripTest.png')
bpy.ops.render.render(write_still=True)
scene.frame_set(1);scene.camera=hero
scene.render.resolution_x=1600;scene.render.resolution_y=1600
scene.render.filepath=os.path.join(OUT,'CyborgHands_Preview.png')
bpy.ops.wm.save_as_mainfile(filepath=BLEND)
print('CYBORG_HANDS_COMPLETE',flush=True)
import runpy
runpy.run_path(os.path.join(OUT,'validate_cyborg_hands.py'),run_name='__main__')
