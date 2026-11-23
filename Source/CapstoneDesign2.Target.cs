// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CapstoneDesign2Target : TargetRules
{
	public CapstoneDesign2Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			WindowsPlatform.Compiler = WindowsCompiler.VisualStudio2026;
		}
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("CapstoneDesign2");
	}
}
