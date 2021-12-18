// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSPredictProjectile : ModuleRules
{
	public FPSPredictProjectile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "Niagara" });
	}
}
