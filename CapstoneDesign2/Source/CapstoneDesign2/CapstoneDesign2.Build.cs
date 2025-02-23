// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CapstoneDesign2 : ModuleRules
{
	public CapstoneDesign2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
