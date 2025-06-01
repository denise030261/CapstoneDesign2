// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CapstoneDesign2 : ModuleRules
{
	public CapstoneDesign2(ReadOnlyTargetRules Target) : base(Target){

        //PublicIncludePaths.AddRange(new string[] { "CapstoneDesign2"});

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "UMG", "Landscape", "LevelSequence", "MovieScene" });
	}
}
