using UnrealBuildTool;

public class IslandGenerator : ModuleRules
{
    public IslandGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "NavigationSystem", 
                "UE5Coro", 
                "GeometryScriptingCore",
                "GeometryFramework"
            }
        );
    }
}