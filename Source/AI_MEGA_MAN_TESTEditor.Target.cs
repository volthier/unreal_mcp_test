using UnrealBuildTool;
using System.Collections.Generic;

public class AI_MEGA_MAN_TESTEditorTarget : TargetRules
{
	public AI_MEGA_MAN_TESTEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.Add("AI_MEGA_MAN_TEST");
	}
}
