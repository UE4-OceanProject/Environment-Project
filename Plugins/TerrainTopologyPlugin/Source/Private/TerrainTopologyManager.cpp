#include "TerrainTopologyManager.h"
#include "TMath.h"
#include "ImageUtils.h"
#include "LandscapeInfoMap.h"
#include "TerrainTopologyPlugin.h"

ATerrainTopologyManager::ATerrainTopologyManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ATerrainTopologyManager::OnConstruction(const FTransform& Transform)
{
}

void ATerrainTopologyManager::BeginPlay()
{
	Super::BeginPlay();
	Start();

}

void ATerrainTopologyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATerrainTopologyManager::Update()
{
	//If settings changed then recreate map.
	if (OnChange())
	{
		Start();
	}
}

bool ATerrainTopologyManager::OnChange()
{
	return true;
}

void ATerrainTopologyManager::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	//FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	//if (PropertyName == GET_MEMBER_NAME_CHECKED(UCustomClass, PropertyName))
	{
	}
	if (OnChange()) {
		Update();
	}
	Super::PostEditChangeProperty(e);
}

void ATerrainTopologyManager::Start()
{
	if (Landscape) {
		m_heights = Load16Bit();
		t_heightmap = CreateRGBA8_TextureFromR8_Array(m_heights);

		//Create color gradients to help visualize the maps.
		m_currentColorMode = m_coloredGradient;
		CreateGradients(m_coloredGradient);

		//If required smooth the heights.
		if (b_smoothHeights)
		{
			SmoothMap(m_heights, smoothHeightsCount);
			t_heightmap_smoothed = CreateRGBA8_TextureFromR8_Array(m_heights);
		}


		//This will jump to the overridden function in the child actor. It will handle setting the texture
		CreateMap();

		//If required smooth the output. This really only works for R8 data right now.
		//Horrible way of re-converting the data back and fourth. Not meant for runtime use..
		if (b_smoothOutput)
		{
			t_output_unsmoothed = t_output;

			TArray<float> output_RAW;
			output_RAW.SetNum(m_height * m_width);

			uint8* DestPtr = static_cast<uint8*>(t_output->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

			for (int y = 0; y < m_height; y++)
			{
				for (int x = 0; x < m_width; x++)
				{
					float test = *DestPtr++;
					output_RAW[y * m_height + x] = test/255;// / TNumericLimits<unsigned short>::Max();
					DestPtr++;
					DestPtr++;
					DestPtr++;
				}

			}

			t_output->PlatformData->Mips[0].BulkData.Unlock();
			t_debug = CreateRGBA8_TextureFromR8_Array(output_RAW);
			SmoothMap(output_RAW, smoothOutputCount);
			t_output = t_output_smoothed= CreateRGBA8_TextureFromR8_Array(output_RAW);
		}
	}
}



TArray<float> ATerrainTopologyManager::Load16Bit(bool bigendian)
{
	LandscapeInfo = CreateLandscapeInfo(false);

	TArray<uint16> heightmap = HeightmapFromLandscape();

	m_width = Resolution_W_H.Width;
	m_height = Resolution_W_H.Height;
	m_size = m_width * m_height;

	m_heights.SetNum(m_size);

	int size = heightmap.Num();// / 2;
	TArray<float>data;
	data.SetNum(size);


	for (int x = 0; x < size; x++)
	{
		//data[x] = (bigendian) ? (heightmap[x].R * 256.0f + heightmap[x].G) : (heightmap[x].R + heightmap[x].G * 256.0f);
		data[x] = heightmap[x];
		data[x] /= TNumericLimits<unsigned short>::Max();
	}

	return data;
}

ULandscapeInfo* ATerrainTopologyManager::CreateLandscapeInfo(bool bMapCheck)
{
	auto test = Landscape->CreateLandscapeInfo();

	// in case this Info object is not initialized yet
	// initialized it with properties from passed actor
	if (test->LandscapeGuid.IsValid() == false ||
		(test->GetLandscapeProxy() == nullptr && ensure(test->LandscapeGuid == Landscape->GetLandscapeGuid())))
	{
		test->LandscapeGuid = Landscape->GetLandscapeGuid();
		test->ComponentSizeQuads = Landscape->ComponentSizeQuads;
		test->ComponentNumSubsections = Landscape->NumSubsections;
		test->SubsectionSizeQuads = Landscape->SubsectionSizeQuads;
		test->DrawScale = Landscape->GetRootComponent() != nullptr ? Landscape->GetRootComponent()->GetRelativeScale3D() : FVector(100.0f);
	}

	// check that passed actor matches all shared parameters
	check(test->LandscapeGuid == Landscape->GetLandscapeGuid());
	check(test->ComponentSizeQuads == Landscape->ComponentSizeQuads);
	check(test->ComponentNumSubsections == Landscape->NumSubsections);
	check(test->SubsectionSizeQuads == Landscape->SubsectionSizeQuads);

	return test;
}

TArray<uint16> ATerrainTopologyManager::HeightmapFromLandscape() //ExportHeightmap()
{
	int32 MinX = MAX_int32;
	int32 MinY = MAX_int32;
	int32 MaxX = -MAX_int32;
	int32 MaxY = -MAX_int32;

	if (!GetLandscapeExtent(MinX, MinY, MaxX, MaxY))
	{
		TArray<uint16>dead;
		return dead;
	}

	TArray<uint16> HeightData;
	HeightData.AddZeroed((MaxX - MinX + 1) * (MaxY - MinY + 1));
	GetHeightDataFast(MinX, MinY, MaxX, MaxY, HeightData.GetData(), 0);
	//HeightmapFormat->Export(*Filename, HeightData, { (uint32)(MaxX - MinX + 1), (uint32)(MaxY - MinY + 1) }, DrawScale * FVector(1, 1, LANDSCAPE_ZSCALE));
	Resolution_W_H = { (uint32)(MaxX - MinX + 1), (uint32)(MaxY - MinY + 1) };

	Scale = LandscapeInfo->DrawScale * FVector(1, 1, LANDSCAPE_ZSCALE);

	return HeightData;

}

bool ATerrainTopologyManager::GetLandscapeExtent(int32& MinX, int32& MinY, int32& MaxX, int32& MaxY) const
{
	MinX = MAX_int32;
	MinY = MAX_int32;
	MaxX = MIN_int32;
	MaxY = MIN_int32;

	// Find range of entire landscape
	for (auto& Comp : Landscape->LandscapeComponents)
	{
		//const ULandscapeComponent* Comp = XYComponentPair.Value;


		MinX = FMath::Min(Comp->SectionBaseX, MinX);
		MinY = FMath::Min(Comp->SectionBaseY, MinY);
		MaxX = FMath::Max(Comp->SectionBaseX + Comp->ComponentSizeQuads - 1, MaxX);
		MaxY = FMath::Max(Comp->SectionBaseY + Comp->ComponentSizeQuads - 1, MaxY);

	}

	return (MinX != MAX_int32);
}

void ATerrainTopologyManager::GetHeightDataFast(const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, uint16* Data, int32 Stride, uint16* NormalData, UTexture2D* InHeightmap)
{
	if (Stride == 0)
	{
		Stride = (1 + X2 - X1);
	}

	TArrayDataAccess<uint16> ArrayStoreData(X1, Y1, Data, Stride);
	if (NormalData)
	{
		TArrayDataAccess<uint16> ArrayNormalData(X1, Y1, NormalData, Stride);
		GetHeightDataTemplFast2(X1, Y1, X2, Y2, ArrayStoreData, InHeightmap, &ArrayNormalData);
	}
	else
	{
		GetHeightDataTemplFast2(X1, Y1, X2, Y2, ArrayStoreData, InHeightmap);
	}
}

void* GetMipData(UTexture2D* Heightmap, int32 MipNum)
{

	uint8* mipData = 0;
	check(MipNum < Heightmap->PlatformData->Mips.Num());

	if (!Heightmap->PlatformData->Mips[MipNum].BulkData.IsLocked())
	{
		mipData = Heightmap->Source.LockMip(MipNum);
	}
	return mipData;
}

void UnlockMipData(UTexture2D* Heightmap, int32 MipNum)
{
	uint8* mipData = 0;
	check(MipNum < Heightmap->PlatformData->Mips.Num());

	//if (Heightmap->PlatformData->Mips[MipNum].BulkData.IsLocked())
	//{
	Heightmap->Source.UnlockMip(MipNum);
	//}
}

TMap<UTexture2D*, TArray<ULandscapeComponent*>> ATerrainTopologyManager::GenerateComponentsPerHeightmaps() const
{
	TMap<UTexture2D*, TArray<ULandscapeComponent*>> ComponentsPerHeightmaps;


	for (ULandscapeComponent* Component : Landscape->LandscapeComponents)
	{
		UTexture2D* ComponentHeightmapTexture = Component->GetHeightmap();
		TArray<ULandscapeComponent*>& ComponentList = ComponentsPerHeightmaps.FindOrAdd(ComponentHeightmapTexture);
		ComponentList.Add(Component);
	}


	return ComponentsPerHeightmaps;
}

TMap<FIntPoint, ULandscapeComponent*> ATerrainTopologyManager::GenerateXYtoCompnentMap() {
	/** Map of the offsets (in component space) to the component. */
	TMap<FIntPoint, ULandscapeComponent*> XYtoComponentMap;
	for (ULandscapeComponent* Component : Landscape->LandscapeComponents) {
		FIntPoint ComponentKey = Component->GetSectionBase() / Component->ComponentSizeQuads;
		XYtoComponentMap.Add(ComponentKey, Component);
	}
	return XYtoComponentMap;
}
template<typename TStoreData>
void ATerrainTopologyManager::GetHeightDataTemplFast(const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TStoreData& StoreData, UTexture2D* InHeightmap, TStoreData* NormalData /*= NULL*/)
{
	if (!LandscapeInfo) return;
	auto ComponentSizeQuads = Landscape->ComponentSizeQuads;
	auto SubsectionSizeQuads = Landscape->SubsectionSizeQuads;
	int32 ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2;
	ALandscape::CalcComponentIndicesNoOverlap(X1, Y1, X2, Y2, ComponentSizeQuads, ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2);

	auto XYtoComponentMap = GenerateXYtoCompnentMap();
	for (int32 ComponentIndexY = ComponentIndexY1; ComponentIndexY <= ComponentIndexY2; ComponentIndexY++)
	{
		for (int32 ComponentIndexX = ComponentIndexX1; ComponentIndexX <= ComponentIndexX2; ComponentIndexX++)
		{
			ULandscapeComponent* Component = XYtoComponentMap.FindRef(FIntPoint(ComponentIndexX, ComponentIndexY));

			if (Component == nullptr)
			{
				continue;
			}

			UTexture2D* Heightmap = InHeightmap != nullptr ? InHeightmap : Component->GetHeightmap(true);

			//FMipInfo* TexDataInfo = NULL;
			FColor* HeightmapTextureData = NULL;
			//			TexDataInfo = GetTextureDataInfo(Heightmap);
						//HeightmapTextureData = (FLinearColor*)TexDataInfo->GetMipData(0);
			HeightmapTextureData = (FColor*)GetMipData(Heightmap, 0);

			// Find coordinates of box that lies inside component
			int32 ComponentX1 = FMath::Clamp<int32>(X1 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY1 = FMath::Clamp<int32>(Y1 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentX2 = FMath::Clamp<int32>(X2 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY2 = FMath::Clamp<int32>(Y2 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);

			// Find subsection range for this box
			int32 SubIndexX1 = FMath::Clamp<int32>((ComponentX1 - 1) / SubsectionSizeQuads, 0, Landscape->NumSubsections/*ComponentNumSubsections*/ - 1);	// -1 because we need to pick up vertices shared between subsections
			int32 SubIndexY1 = FMath::Clamp<int32>((ComponentY1 - 1) / SubsectionSizeQuads, 0, Landscape->NumSubsections/*ComponentNumSubsections*/ - 1);
			int32 SubIndexX2 = FMath::Clamp<int32>(ComponentX2 / SubsectionSizeQuads, 0, Landscape->NumSubsections/*ComponentNumSubsections*/ - 1);
			int32 SubIndexY2 = FMath::Clamp<int32>(ComponentY2 / SubsectionSizeQuads, 0, Landscape->NumSubsections/*ComponentNumSubsections*/ - 1);

			for (int32 SubIndexY = SubIndexY1; SubIndexY <= SubIndexY2; SubIndexY++)
			{
				for (int32 SubIndexX = SubIndexX1; SubIndexX <= SubIndexX2; SubIndexX++)
				{
					// Find coordinates of box that lies inside subsection
					int32 SubX1 = FMath::Clamp<int32>(ComponentX1 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY1 = FMath::Clamp<int32>(ComponentY1 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);
					int32 SubX2 = FMath::Clamp<int32>(ComponentX2 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY2 = FMath::Clamp<int32>(ComponentY2 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);

					// Update texture data for the box that lies inside subsection
					for (int32 SubY = SubY1; SubY <= SubY2; SubY++)
					{
						for (int32 SubX = SubX1; SubX <= SubX2; SubX++)
						{
							int32 LandscapeX = SubIndexX * SubsectionSizeQuads + ComponentIndexX * ComponentSizeQuads + SubX;
							int32 LandscapeY = SubIndexY * SubsectionSizeQuads + ComponentIndexY * ComponentSizeQuads + SubY;

							// Find the texture data corresponding to this vertex
							int32 SizeU = Heightmap->Source.GetSizeX();
							int32 SizeV = Heightmap->Source.GetSizeY();
							int32 HeightmapOffsetX = Component->HeightmapScaleBias.Z * (float)SizeU;
							int32 HeightmapOffsetY = Component->HeightmapScaleBias.W * (float)SizeV;

							int32 TexX = HeightmapOffsetX + (SubsectionSizeQuads + 1) * SubIndexX + SubX;
							int32 TexY = HeightmapOffsetY + (SubsectionSizeQuads + 1) * SubIndexY + SubY;
							FColor& TexData = HeightmapTextureData[TexX + TexY * SizeU];

							uint16 Height = (((uint16)TexData.R) << 8) | TexData.G;
							StoreData.Store(LandscapeX, LandscapeY, Height);
							if (NormalData)
							{
								uint16 Normals = (((uint16)TexData.B) << 8) | TexData.A;
								NormalData->Store(LandscapeX, LandscapeY, Normals);
							}
						}
					}
				}
			}
			UnlockMipData(Heightmap, 0);
		}
	}
}
template<typename TStoreData>
void ATerrainTopologyManager::GetHeightDataTemplFast2(const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TStoreData& StoreData, UTexture2D* InHeightmap, TStoreData* NormalData /*= NULL*/)
{
	if (!LandscapeInfo) return;
	auto ComponentSizeQuads = Landscape->ComponentSizeQuads;
	auto SubsectionSizeQuads = Landscape->SubsectionSizeQuads;
	auto ComponentNumSubsections = LandscapeInfo->ComponentNumSubsections;
	int32 ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2;
	ALandscape::CalcComponentIndicesNoOverlap(X1, Y1, X2, Y2, ComponentSizeQuads, ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2);

	for (int32 ComponentIndexY = ComponentIndexY1; ComponentIndexY <= ComponentIndexY2; ComponentIndexY++)
	{
		for (int32 ComponentIndexX = ComponentIndexX1; ComponentIndexX <= ComponentIndexX2; ComponentIndexX++)
		{
			ULandscapeComponent* Component = LandscapeInfo->XYtoComponentMap.FindRef(FIntPoint(ComponentIndexX, ComponentIndexY));

			if (Component == nullptr)
			{
				continue;
			}

			UTexture2D* Heightmap = InHeightmap != nullptr ? InHeightmap : Component->GetHeightmap(true);

			FLandscapeTextureDataInfo* TexDataInfo = NULL;
			FColor* HeightmapTextureData = NULL;
			//TexDataInfo = GetTextureDataInfo(Heightmap);
			//HeightmapTextureData = (FColor*)TexDataInfo->GetMipData(0);
			HeightmapTextureData = (FColor*)GetMipData(Heightmap, 0);

			// Find coordinates of box that lies inside component
			int32 ComponentX1 = FMath::Clamp<int32>(X1 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY1 = FMath::Clamp<int32>(Y1 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentX2 = FMath::Clamp<int32>(X2 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY2 = FMath::Clamp<int32>(Y2 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);

			// Find subsection range for this box
			int32 SubIndexX1 = FMath::Clamp<int32>((ComponentX1 - 1) / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);	// -1 because we need to pick up vertices shared between subsections
			int32 SubIndexY1 = FMath::Clamp<int32>((ComponentY1 - 1) / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);
			int32 SubIndexX2 = FMath::Clamp<int32>(ComponentX2 / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);
			int32 SubIndexY2 = FMath::Clamp<int32>(ComponentY2 / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);

			for (int32 SubIndexY = SubIndexY1; SubIndexY <= SubIndexY2; SubIndexY++)
			{
				for (int32 SubIndexX = SubIndexX1; SubIndexX <= SubIndexX2; SubIndexX++)
				{
					// Find coordinates of box that lies inside subsection
					int32 SubX1 = FMath::Clamp<int32>(ComponentX1 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY1 = FMath::Clamp<int32>(ComponentY1 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);
					int32 SubX2 = FMath::Clamp<int32>(ComponentX2 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY2 = FMath::Clamp<int32>(ComponentY2 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);

					// Update texture data for the box that lies inside subsection
					for (int32 SubY = SubY1; SubY <= SubY2; SubY++)
					{
						for (int32 SubX = SubX1; SubX <= SubX2; SubX++)
						{
							int32 LandscapeX = SubIndexX * SubsectionSizeQuads + ComponentIndexX * ComponentSizeQuads + SubX;
							int32 LandscapeY = SubIndexY * SubsectionSizeQuads + ComponentIndexY * ComponentSizeQuads + SubY;

							// Find the texture data corresponding to this vertex
							int32 SizeU = Heightmap->Source.GetSizeX();
							int32 SizeV = Heightmap->Source.GetSizeY();
							int32 HeightmapOffsetX = Component->HeightmapScaleBias.Z * (float)SizeU;
							int32 HeightmapOffsetY = Component->HeightmapScaleBias.W * (float)SizeV;

							int32 TexX = HeightmapOffsetX + (SubsectionSizeQuads + 1) * SubIndexX + SubX;
							int32 TexY = HeightmapOffsetY + (SubsectionSizeQuads + 1) * SubIndexY + SubY;
							FColor& TexData = HeightmapTextureData[TexX + TexY * SizeU];

							uint16 Height = (((uint16)TexData.R) << 8) | TexData.G;
							StoreData.Store(LandscapeX, LandscapeY, Height);
							if (NormalData)
							{
								uint16 Normals = (((uint16)TexData.B) << 8) | TexData.A;
								NormalData->Store(LandscapeX, LandscapeY, Normals);
							}
						}
					}
				}
			}
			UnlockMipData(Heightmap, 0);
		}
	}
}



void ATerrainTopologyManager::SmoothMap(TArray<float>& map, int loopCount)
{
	TArray<float> heights;
	heights.SetNum(m_width * m_height);
	//auto gaussianKernel5 = TArray<TArray<float>>
	//{
	//	{1, 4, 6, 4, 1},
	//	{4, 16, 24, 16, 4},
	//	{6, 24, 36, 24, 6},
	//	{4, 16, 24, 16, 4},
	//	{1, 4, 6, 4, 1}
	//};

	//float gaussScale = 1.0f / 256.0f;

	auto gaussianKernel5 = TArray<TArray<float>>
	{
		{1, 4, 7, 4, 1},
		{4, 16, 26, 16, 4},
		{7, 26, 36, 26, 7},
		{4, 16, 26, 16, 4},
		{1, 4, 7, 4, 1}
	};

	float gaussScale = 1.0f / 273.0f;
	for (int a = 0; a < loopCount; a++) {

		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				float sum = 0;

				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						int xi = x - 2 + i;
						int yi = y - 2 + j;

						sum += GetNormalizedHeight(xi, yi, map) * gaussianKernel5[i][j] * gaussScale;
					}
				}

				heights[x + y * m_width] = sum;
			}
		}
		map = heights;
	}
}

float ATerrainTopologyManager::GetNormalizedHeight(int x, int y, TArray<float>& map)
{

	x = FMath::Clamp(x, 0, m_width - 1);
	y = FMath::Clamp(y, 0, m_height - 1);

	return map[y * m_height + x];
}



void ATerrainTopologyManager::CreateMap()
{

}

FVector2D* ATerrainTopologyManager::GetFirstDerivative(int x, int y)
{
	float w = m_cellLength;
	float z1 = GetHeight(x - 1, y + 1);
	float z2 = GetHeight(x + 0, y + 1);
	float z3 = GetHeight(x + 1, y + 1);
	float z4 = GetHeight(x - 1, y + 0);
	float z6 = GetHeight(x + 1, y + 0);
	float z7 = GetHeight(x - 1, y - 1);
	float z8 = GetHeight(x + 0, y - 1);
	float z9 = GetHeight(x + 1, y - 1);

	//p, q
	float zx = (z3 + z6 + z9 - z1 - z4 - z7) / (6.0f * w);
	float zy = (z1 + z2 + z3 - z7 - z8 - z9) / (6.0f * w);

	return new FVector2D(-zx, -zy);
}

void ATerrainTopologyManager::GetDerivatives(int x, int y, FVector2D*& d1, FVector*& d2)
{
	float w = m_cellLength;
	float w2 = w * w;
	float z1 = GetHeight(x - 1, y + 1);
	float z2 = GetHeight(x + 0, y + 1);
	float z3 = GetHeight(x + 1, y + 1);
	float z4 = GetHeight(x - 1, y + 0);
	float z5 = GetHeight(x + 0, y + 0);
	float z6 = GetHeight(x + 1, y + 0);
	float z7 = GetHeight(x - 1, y - 1);
	float z8 = GetHeight(x + 0, y - 1);
	float z9 = GetHeight(x + 1, y - 1);

	//p, q
	float zx = (z3 + z6 + z9 - z1 - z4 - z7) / (6.0f * w);
	float zy = (z1 + z2 + z3 - z7 - z8 - z9) / (6.0f * w);

	//r, t, s
	float zxx = (z1 + z3 + z4 + z6 + z7 + z9 - 2.0f * (z2 + z5 + z8)) / (3.0f * w2);
	float zyy = (z1 + z2 + z3 + z7 + z8 + z9 - 2.0f * (z4 + z5 + z6)) / (3.0f * w2);
	float zxy = (z3 + z7 - z1 - z9) / (4.0f * w2);

	d1 = new FVector2D(-zx, -zy);
	d2 = new FVector(-zxx, -zyy, -zxy); //is zxy or -zxy?
}

FLinearColor ATerrainTopologyManager::GetPixel(TArray<FLinearColor> tex, int x, int y, int Width, int Height)
{
	x = FMath::Clamp(x, 0, Width - 1);
	y = FMath::Clamp(y, 0, Height - 1);
	return tex[x + y * Width];
}

FLinearColor ATerrainTopologyManager::GetPixelBilinear(TArray<FColor> data, double x, double y, int Width, int Height)
{
	int xMin, xMax, yMin, yMax;
	double xfloat, yfloat;
	FLinearColor c, h1, h2;

	auto xfloatx = (Width - 1) * x;
	auto yfloaty = (Height - 1) * y;

	xfloat = FMath::Clamp(xfloatx, 0.0, Width - 1 * 1.0);
	yfloat = FMath::Clamp(yfloaty, 0.0, Height - 1 * 1.0);

	xMin = FMath::FloorToInt(xfloat);
	xMax = FMath::CeilToInt(xfloat);

	yMin = (int)FMath::FloorToInt(yfloat);
	yMax = FMath::CeilToInt(yfloat);

	h1 = FLinearColor::LerpUsingHSV(data[xMin + yMin * Width], data[xMax + yMin * Width], (float)FractionalPart(xfloat));
	h2 = FLinearColor::LerpUsingHSV(data[xMin + yMax * Width], data[xMax + yMax * Width], (float)FractionalPart(xfloat));
	c = FLinearColor::LerpUsingHSV(h1, h2, (float)FractionalPart(yfloat));

	return c;
}
//FLinearColor ATerrainTopologyManager::GetPixelBilinear2(TArray<FLinearColor> data, double x, double y, int Width, int Height)
//{
//	int xMin, xMax, yMin, yMax;
//	double xfloat, yfloat;
//	UnityEngine.Color c, h1, h2;
//
//	xfloat = (Width - 1) * x;
//	yfloat = (Height - 1) * y;
//
//	xMin = (int)Math.Floor(xfloat);
//	xMax = (int)Math.Ceiling(xfloat);
//
//	yMin = (int)Math.Floor(yfloat);
//	yMax = (int)Math.Ceiling(yfloat);
//
//	h1 = UnityEngine.Color.Lerp(GetPixel(xMin, yMin), GetPixel(xMax, yMin), (float)FractionalPart(xfloat));
//	h2 = UnityEngine.Color.Lerp(GetPixel(xMin, yMax), GetPixel(xMax, yMax), (float)FractionalPart(xfloat));
//	c = UnityEngine.Color.Lerp(h1, h2, (float)FractionalPart(yfloat));
//	return c;
//}
float ATerrainTopologyManager::FractionalPart(float input)
{
	int floor = (int)FMath::Floor(input);
	return input - floor;
}

FLinearColor ATerrainTopologyManager::Colorize(float v, float exponent, bool nonNegative)
{
	if (exponent > 0)
	{
		float sign = TMath::SignOrZero(v);
		float pow = FMath::Pow(10, exponent);
		//I think this is Loge
		float log = FMath::Loge(1.0f + pow * FMath::Abs(v));
		v = sign * log;
	}

	if (nonNegative)
	{
		return GetPixelBilinear(m_gradient, v, 0, m_gradient.Num(), 1);
	}
	else
	{
		if (v > 0)
		{
			return GetPixelBilinear(m_posGradient, v, 0, m_posGradient.Num(), 1);
		}
		else
		{
			return GetPixelBilinear(m_negGradient, -v, 0, m_posGradient.Num(), 1);
		}
	}
}

void ATerrainTopologyManager::CreateGradients(bool colored)
{
	if (colored)
	{

		m_gradient = GRADIENT_data[GRADIENT::COOL_WARM];
		m_posGradient = GRADIENT_data[GRADIENT::WARM];
		m_negGradient = GRADIENT_data[GRADIENT::COOL];
	}
	else
	{
		m_gradient = GRADIENT_data[GRADIENT::BLACK_WHITE];
		m_posGradient = GRADIENT_data[GRADIENT::GREY_WHITE];
		m_negGradient = GRADIENT_data[GRADIENT::GREY_BLACK];
	}
}

float ATerrainTopologyManager::GetHeight(int x, int y)
{
	return GetNormalizedHeight(x, y, m_heights) * m_terrainHeight;
}



UTexture2D* ATerrainTopologyManager::CreateTexture(const int32 SrcWidth, const int32 SrcHeight)
{
	// Create the texture
	return UTexture2D::CreateTransient(
		SrcWidth,
		SrcHeight,
		PF_B8G8R8A8
	);
};

UTexture2D* ATerrainTopologyManager::CreateRGBA8_TextureFromR8_Array(TArray<float>& data)
{
	UTexture2D* tex = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(tex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	for (int32 y = 0; y < m_height; y++)
	{
		for (int32 x = 0; x < m_width; x++)
		{
			float color = data[y * m_height + x];
			FColor UEColor = FLinearColor(color, color, color, 1).ToFColor(false);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}
	}

	tex->PlatformData->Mips[0].BulkData.Unlock();
	tex->UpdateResource();
	return tex;
}

UTexture2D* ATerrainTopologyManager::TextureFromRAW(const int32 SrcWidth, const int32 SrcHeight, const TArray<FLinearColor>& SrcData, const bool UseAlpha, enum TextureFilter sampleMode)
{
	// Create the texture
	auto gradientTex = UTexture2D::CreateTransient(
		SrcWidth,
		SrcHeight,
		PF_B8G8R8A8
	);

	gradientTex->Filter = sampleMode;


	uint8* MipData = static_cast<uint8*>(gradientTex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	// Create base mip.
	uint8* DestPtr = NULL;
	const FLinearColor* SrcPtr = NULL;
	for (int32 y = 0; y < SrcHeight; y++)
	{
		DestPtr = &MipData[(SrcHeight - 1 - y) * SrcWidth * sizeof(FLinearColor)];
		SrcPtr = const_cast<FLinearColor*>(&SrcData[(SrcHeight - 1 - y) * SrcWidth]);
		for (int32 x = 0; x < SrcWidth; x++)
		{
			*DestPtr++ = SrcPtr->B;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->R;
			if (UseAlpha)
			{
				*DestPtr++ = SrcPtr->A;
			}
			else
			{
				*DestPtr++ = 0xFF;
			}
			SrcPtr++;
		}
	}



	gradientTex->PlatformData->Mips[0].BulkData.Unlock();
	gradientTex->UpdateResource();

	return gradientTex;
}

