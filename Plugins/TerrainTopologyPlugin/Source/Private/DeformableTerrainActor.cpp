#include "DeformableTerrainActor.h"
#include "UObject/Object.h"
//#include "Materials/MaterialInstanceDynamic.h"
//#include "LandscapeStreamingProxy.h"
//Only for testing / logging purposes
#include <iostream>
#include <fstream>
//Sets default values
ADeformableTerrainActor::ADeformableTerrainActor()
{
	PrimaryActorTick.bCanEverTick = false;
	DepthCaptureCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("DepthCaptureCamera"));
	RootComponent = DepthCaptureCamera;
	HeightmapRenderTarget = NewObject<UTextureRenderTarget2D>();
}
ADeformableTerrainActor* ADeformableTerrainActor::Get()
{
	return this;
}
void ADeformableTerrainActor::EditorInit()
{
	Init();
}
void ADeformableTerrainActor::ImportHeightFromRenderTarget()
{
	uint64 startTime;
	//uint64 endTime;
	ALandscape* Landscape = BaseLandscape;
	ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();
	HeightData.Empty();
	startTime = FPlatformTime::Cycles64();
	RenderTargetResource->ReadFloat16Pixels(OutputRTHeightmap, CubeFace_MAX);
	for (FFloat16Color Color : OutputRTHeightmap)
	{
		uint16 Height = (uint16)((Color.R.Encoded - RTDepthLowerBound) * scaler) + HeightmapLowerBound;
		HeightData.Add(Height);

	}

	TArray<uint16>FlippedHeightData;
	FlippedHeightData.Reserve(HeightData.Num());
	uint16* data = HeightData.GetData();
	for (int x = 0, width = RenderTargetResource->GetSizeX(); x < width; ++x)
	{
		for (int y = 0, height = RenderTargetResource->GetSizeY(); y < height; ++y)
		{
			FlippedHeightData.Add(data[width - 1 - x + (height - 1 - y) * width]);
		}
	}
	FHeightmapAccessor<false>HeightmapAccessor(LandscapeInfo);

	HeightmapAccessor.SetData(MinX, MinY, MaxX, MaxY, FlippedHeightData.GetData());
	long double error = CalculateError(FlippedHeightData);
	//Clean up arrays
	HeightData.Empty();
	FlippedHeightData.Empty();
	OutputRTHeightmap.Empty();
}

void ADeformableTerrainActor::ResetHeightmapToDefault()
{
	ResetHeightMap();
}
//Called when the game starts or whenspawned
void ADeformableTerrainActor::BeginPlay()
{
	Super::BeginPlay();
	Init();
}


void ADeformableTerrainActor::ResetHeightMap()
{
	ULandscapeInfo* LandscapeInfo = BaseLandscape->GetLandscapeInfo();
	FHeightmapAccessor<false>HeightmapAccessor(LandscapeInfo);
	TArray<uint16>OriginalHeightData2;
	HeightmapAccessor.SetData(MinX, MinY, MaxX, MaxY, OriginalHeightData.GetData());
}


//ULandscapeInfo* ADeformableTerrainActor::CreateLandscapeInfo(bool bMapCheck)
//{
//	ULandscapeInfo* LandscapeInfo = nullptr;
//	auto LandscapeGuid = BaseLandscape->GetLandscapeGuid();
//	check(LandscapeGuid.IsValid());
//	UWorld* OwningWorld = GetWorld();
//	check(OwningWorld);
//
//	auto& LandscapeInfoMap = ULandscapeInfoMap::GetLandscapeInfoMap(OwningWorld);
//	LandscapeInfo = LandscapeInfoMap.Map.FindRef(LandscapeGuid);
//
//	if (!LandscapeInfo)
//	{
//		check(!HasAnyFlags(RF_BeginDestroyed));
//		LandscapeInfo = NewObject<ULandscapeInfo>(GetTransientPackage(), NAME_None, RF_Transactional | RF_Transient);
//		LandscapeInfoMap.Modify(false);
//		LandscapeInfoMap.Map.Add(LandscapeGuid, LandscapeInfo);
//	}
//	check(LandscapeInfo);
//	LandscapeInfo->RegisterActor(BaseLandscape, bMapCheck);
//
//	return LandscapeInfo;
//}
void ADeformableTerrainActor::Init()
{

	//CreateLandscapeInfo(false);
	ULandscapeInfo::RecreateLandscapeInfo(GetWorld(), false);
	ULandscapeInfo* LandscapeInfo = BaseLandscape->GetLandscapeInfo();

	LandscapeInfo->RegisterActor(BaseLandscape,false);
	LandscapeInfo->LandscapeGuid = BaseLandscape->GetLandscapeGuid();

	LandscapeInfo->ComponentSizeQuads = BaseLandscape->ComponentSizeQuads;

	LandscapeInfo->ComponentNumSubsections = BaseLandscape->NumSubsections;

	LandscapeInfo->SubsectionSizeQuads = BaseLandscape->SubsectionSizeQuads;

	LandscapeInfo->DrawScale = BaseLandscape->GetRootComponent() != nullptr ? BaseLandscape->GetRootComponent()->GetRelativeScale3D() : FVector(100.0f);

	//LandscapeInfo->XYtoComponentMap.Add();

	LandscapeInfo->GetLandscapeExtent(MinX, MinY, MaxX, MaxY);
	RenderTargetResource = HeightmapRenderTarget->GameThread_GetRenderTargetResource();
	SampleRect = FIntRect(0, 0, FMath::Min(1 + MaxX - MinX, HeightmapRenderTarget->SizeX),
		FMath::Min(1 + MaxY - MinY, HeightmapRenderTarget->SizeY));

	//Make sure arrays are clean
	OriginalHeightData.Empty();
	HeightData.Empty();
	OutputRTHeightmap.Empty();

	//Reserve memory for heightmap arrays
	int32 HeightmapSize = SampleRect.Width() * SampleRect.Height();
	OriginalHeightData.Reserve(HeightmapSize);
	OutputRTHeightmap.Reserve(HeightmapSize);
	HeightData.Reserve(HeightmapSize);
	FHeightmapAccessor<false> HeightmapAccessor(LandscapeInfo);

	//Store the original unmodified height data
	HeightmapAccessor.GetDataFast(MinX, MinY, MaxX, MaxY, OriginalHeightData.GetData());



	HeightmapLowerBound = MAX_int32;
	int32 HeightmapHigherBound = MIN_int32;
	for (int i = 0; i < HeightmapSize; ++i)
	{
		auto a = OriginalHeightData.GetData()[i];
		if (a > HeightmapHigherBound)
		{
			HeightmapHigherBound = a;
		}
		if (a < HeightmapLowerBound)
		{
			HeightmapLowerBound = a;
		}
	}
	//Place the camera under the terrain
	FTransform ModifiedTransform = GetTransform();
	FVector Origin, BoundsExtent;
	BaseLandscape->GetActorBounds(false, Origin, BoundsExtent);
	DepthCaptureCamera->OrthoWidth = BoundsExtent.X * 2;
	SetActorTransform(ModifiedTransform);
	RenderTargetResource->ReadFloat16Pixels(OutputRTHeightmap, CubeFace_MAX);
	int32 RTHeightmapLowerBound = MAX_int32, RTHeightmapHigherBound = MIN_int32;
	for (FFloat16Color Color : OutputRTHeightmap)
	{
		uint16 Height = (uint16)Color.R.Encoded;
		HeightData.Add(Height);
		if ((uint16)Color.R.Encoded > RTHeightmapHigherBound)
		{
			RTHeightmapHigherBound = (uint16)Color.R.Encoded;
		}
		if ((uint16)Color.R.Encoded < RTHeightmapLowerBound)
		{
			RTHeightmapLowerBound = (uint16)Color.R.Encoded;
		}
	}



	int32 temp = RTHeightmapHigherBound - RTHeightmapLowerBound;
	int32 temp2 = HeightmapHigherBound - HeightmapLowerBound;
	RTDepthLowerBound = RTHeightmapLowerBound;
	scaler = (float)temp2 / (float)temp;
}

long double ADeformableTerrainActor::CalculateError(TArray<uint16>& newArray)
{
	long double count = newArray.Num();
	uint16* origData = OriginalHeightData.GetData();
	uint16* newData = newArray.GetData();
	long double averageError = 0.0;
	for (int i = 0; i < count; ++i)
	{
		averageError += abs((long double)origData[i] - (long double)newData[i]) / count;
	}
	return averageError;
}
//Called every frame
void ADeformableTerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}