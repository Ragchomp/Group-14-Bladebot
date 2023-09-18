// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BladeBot : ModuleRules
{
	public BladeBot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "GeometryCollectionEngine", "Niagara", "UMG", "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent", "UMGEditor", "UMGEditor", "NavigationSystem" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
