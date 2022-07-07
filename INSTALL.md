## Install Instructions

* Download the entire project from github via git (Do NOT download as zip, you won't get the plugins because they are sub modules or "sub repos")

*Right click the .uproject file, and click "Generate Project Files"

*Now you can open the project. Click yes to rebuild when asked.

## If adding just the plugins to your own project modify DefaultEngine.ini to include:
[/Script/Engine.RendererSettings]
r.MobileHDR=True
r.GenerateMeshDistanceFields=True
r.SeparateTranslucency=True
r.CustomDepth=3
r.AllowGlobalClipPlane=True
r.CustomDepthTemporalAAJitter=True
r.DefaultFeature.AntiAliasing=2
r.TemporalAA.Upsampling=True
r.SupportSkyAtmosphereAffectsHeightFog=True
r.SupportAtmosphericFog=True
r.SupportSkyAtmosphere=True

[/Script/Engine.Engine]
NearClipPlane=10.000000

#### How to package (and include OceanPlugin in the build)

* Close your project and UE5

* Edit your `.uproject` and add this:

  ```json
  "Plugins": [
      {
          "Name": "OceanPlugin",
          "Type": "Runtime",
          "LoadingPhase": "PreDefault",
          "Enabled": true
      }
  ]
  ```

  So your `.uproject` may look like this

  ```json
  {
  	"FileVersion": 3,
  	"EngineAssociation": "5.0",
  	"Category": "",
  	"Description": "",
  	"Plugins": [
  		{
  			"Name": "OceanPlugin",
  			"Type": "Runtime",
  			"LoadingPhase": "PreDefault",
  			"Enabled": true
  		}
  	]
  }
  ```