#pragma once

//#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainTopologyManager.generated.h"


//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS(Blueprintable)
class ATerrainTopologyManager : public AActor
{
	GENERATED_BODY()

public:
	ATerrainTopologyManager(const class FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};