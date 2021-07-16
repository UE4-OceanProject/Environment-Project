#include "TerrainTopologyManager.h"
#include "TerrainTopologyPlugin.h"

ATerrainTopologyManager::ATerrainTopologyManager(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATerrainTopologyManager::OnConstruction(const FTransform& Transform)
{

}

void ATerrainTopologyManager::BeginPlay()
{
	Super::BeginPlay();

}

void ATerrainTopologyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

