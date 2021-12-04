// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

#include "TerrainTopologyPlugin.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY(LogTerrainTopologyPlugin);

void FTerrainTopologyPlugin::StartupModule()
{
	UE_LOG(LogTerrainTopologyPlugin, Display, TEXT("%s:: StartupModle() Register OnWorldCreated delegate"), *PLUGIN_FUNC_LINE);

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	//Auto create our TerrainTopologyManager
	//This is called everytime UWorld is created, which is a lot in the editor (every opened BP gets a UWorld)
//	FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FTerrainTopologyPlugin::InitSingletonActor);
	UE_LOG(LogTerrainTopologyPlugin, Display, TEXT("%s:: Module started"), *PLUGIN_FUNC_LINE);
}

void FTerrainTopologyPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
}
//
//void FTerrainTopologyPlugin::EnforceSingletonActor(UWorld* World)
//{
//	//Make sure there is only one instance of this actor!
//	//Actor is not blueprintable, but users will find other ways!!
//	bool bFoundFirstInstance = false;
//	for (TActorIterator<ATerrainTopologyManager> ActorItr(World); ActorItr; ++ActorItr)
//	{
//		if (bFoundFirstInstance == false)
//		{
//			bFoundFirstInstance = true;
//		}
//		else
//		{
//			UE_LOG(LogTerrainTopologyPlugin, Display, TEXT("%s:: found more than one TerrainTopologyPlugin destroying..."), *PLUGIN_FUNC_LINE);
//			ActorItr->Destroy();
//		}
//	}
//}
//
//ATerrainTopologyManager * FTerrainTopologyPlugin::SpawnSingletonActor(UWorld* World)
//{
//	FActorSpawnParameters SpawnInfo;
//	ATerrainTopologyManager* TerrainTopologyManager = World->SpawnActor<ATerrainTopologyManager>(ATerrainTopologyManager::StaticClass(), FTransform::Identity, FActorSpawnParameters());
//	if (!TerrainTopologyManager)
//		UE_LOG(LogTerrainTopologyPlugin, Display, TEXT("%s:: Failed to spawn Singleton!"), *PLUGIN_FUNC_LINE);
//	return TerrainTopologyManager;
//}
//
//void FTerrainTopologyPlugin::InitSingletonActor(UWorld* World, const UWorld::InitializationValues IVS)
//{
//	//Make sure we are in the correct UWorld!
//	if ((World->WorldType == EWorldType::Game) || (World->WorldType == EWorldType::PIE) || (World->WorldType == EWorldType::Editor))
//	{
//		//If we already have a TerrainTopologyManagerEditorActor in the editor level, do not spawn another one
//		//This also auto spawns a TerrainTopologyManagerActor in the game world, if the user somehow sneaks a map in
//		//that has not been opened while the plugin was active!
//
//		EnforceSingletonActor(World);
//
//		for (TActorIterator<ATerrainTopologyManager> ActorItr(World); ActorItr; ++ActorItr)
//		{
//			//If TerrainTopologyManager already exists
//			return;
//		}
//
//		//Spawn TerrainTopologyManager since there isn't one already
//		UE_LOG(LogTerrainTopologyPlugin, Display, TEXT("%s:: No TerrainTopologyPlugin found... spawning..."), *PLUGIN_FUNC_LINE);
//		SpawnSingletonActor(World);
//	}
//}
//
//ATerrainTopologyManager * FTerrainTopologyPlugin::GetSingletonActor(UObject* WorldContextObject)
//{
//	UWorld* World = WorldContextObject->GetWorld();
//	if ((World->WorldType == EWorldType::EditorPreview) || (World->WorldType == EWorldType::GamePreview))
//		return NULL;
//	if (World->bIsRunningConstructionScript)
//		return NULL;
//
//	EnforceSingletonActor(World);
//
//	for (TActorIterator<ATerrainTopologyManager> ActorItr(World); ActorItr; ++ActorItr)
//	{
//		return *ActorItr;
//	}
//
//	//In the impossible case that we don't have an actor, spawn one!
//	UE_LOG(LogTerrainTopologyPlugin, Display, TEXT("%s:: Somehow we don't have a TerrainTopologyPlugin even after all the safeguards... spawning..."), *PLUGIN_FUNC_LINE);
//	return SpawnSingletonActor(World);
//}

IMPLEMENT_MODULE(FTerrainTopologyPlugin, TerrainTopologyPlugin)

