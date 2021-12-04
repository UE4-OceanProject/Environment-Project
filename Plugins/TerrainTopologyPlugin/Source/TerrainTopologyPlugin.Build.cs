// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

using UnrealBuildTool;
using System.IO;

public class TerrainTopologyPlugin : ModuleRules
{
    public TerrainTopologyPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePaths.AddRange(new string[] { });

        PrivateIncludePaths.AddRange(new string[] { });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "Landscape",
            "Foliage",
            "CoreUObject"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject",
            "Engine",
            "RHI",
            "RenderCore"
        });

        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}

