#pragma once
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Landscape.h"
#include "LandscapeEdit.h"
//#include "LandscapeProxy.h"
#include "LandscapeInfo.h"
//#include "LandscapeInfoMap.h"
//#include "LandscapeEditorUtils.h"
#include "Engine/TextureRenderTarget2D.h"
//#include "../Foliage/FoliageClasses.h"
#include "InstancedFoliageActor.h"
//#include "TerrainTopologyPlugin.h"
#include "DeformableTerrainActor.generated.h"
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ADeformableTerrainActor : public AActor
{
	GENERATED_BODY()

public:
	//Setsdefaultvaluesforthisactor’sproperties
	ADeformableTerrainActor();

	UFUNCTION(BlueprintPure,
		Category = "DeformableTerrain",
		DisplayName = "GetDeformableTerrainActor",
		meta = (WorldContext = "WorldContextObject",
			UnsafeDuringActorConstruction = "true"))
		ADeformableTerrainActor* Get();
	UFUNCTION(BlueprintCallable, CallInEditor)
		void EditorInit();
	UFUNCTION(BlueprintCallable)

		void ImportHeightFromRenderTarget();
	UFUNCTION(BlueprintCallable)
		void ResetHeightmapToDefault();

protected:
	UPROPERTY(VisibleAnywhere, Category = "BaseSettings")
		USceneCaptureComponent2D* DepthCaptureCamera;
	UPROPERTY(EditAnywhere, Category = "BaseSettings")
		ALandscape* BaseLandscape;
	UPROPERTY(EditAnywhere, Category = "BaseSettings")
		UTextureRenderTarget2D* HeightmapRenderTarget;


private:
	TArray<uint16 >OriginalHeightData;
	int32 MinX, MinY, MaxX, MaxY;
	FTextureRenderTargetResource* RenderTargetResource;
	FIntRect SampleRect;
	TArray<uint16 >HeightData;
	TArray<FFloat16Color>OutputRTHeightmap;
	float scaler = 1;
	int32 RTDepthLowerBound = 0;
	int32 HeightmapLowerBound = MAX_int32;
	void ResetHeightMap();
	//ULandscapeInfo* CreateLandscapeInfo(bool bMapCheck);
	void Init();
	long double CalculateError(TArray<uint16 >& newArray);

public:
	//virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


};

