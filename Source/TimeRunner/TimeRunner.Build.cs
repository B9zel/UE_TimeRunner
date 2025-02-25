// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TimeRunner : ModuleRules
{
    public TimeRunner(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.Add("TimeRunner");


        if (Target.Configuration != UnrealTargetConfiguration.Shipping || Target.Type == TargetType.Editor)
        {
            PublicDefinitions.Add("AUTOMATION_TEST=1");
        }
        else
        {
            PublicDefinitions.Add("AUTOMATION_TEST=0");
        }

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
