// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CapstoneDesign2 : ModuleRules
{
	public CapstoneDesign2(ReadOnlyTargetRules Target) : base(Target){

        //PublicIncludePaths.AddRange(new string[] { "CapstoneDesign2"});

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new string[] { "MoviePlayer", "Slate", "SlateCore", "AssetRegistry", "RenderCore", "RHI" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "UMG", "Landscape", "LevelSequence", "MovieScene" });
	}
}
