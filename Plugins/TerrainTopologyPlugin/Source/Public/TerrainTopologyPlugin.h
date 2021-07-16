// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

#pragma once

#include "TerrainTopologyManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTerrainTopologyPlugin, Display, All);

#define PLUGIN_FUNC (FString(__FUNCTION__))              // Current Class Name + Function Name where this is called
#define PLUGIN_LINE (FString::FromInt(__LINE__))         // Current Line Number in the code where this is called
#define PLUGIN_FUNC_LINE (PLUGIN_FUNC + "(Line: " + PLUGIN_LINE + ")") // Current Class and Line Number where this is called!


class TERRAINTOPOLOGYPLUGIN_API FTerrainTopologyPlugin : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void EnforceSingletonActor(UWorld* World);
	ATerrainTopologyManager * SpawnSingletonActor(UWorld* World);
	void InitSingletonActor(UWorld* World, const UWorld::InitializationValues IVS);

	ATerrainTopologyManager * GetSingletonActor(UObject* WorldContextObject);

	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline FTerrainTopologyPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked< FTerrainTopologyPlugin >("TerrainTopologyPlugin");
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("TerrainTopologyPlugin");
	}
};

