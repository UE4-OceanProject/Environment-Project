#pragma once

//#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
////#include <string>
////#include <vector>
////#include <limits>
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "LandscapeProxy.h"
#include "TerrainTopologyManager.generated.h"

#define LANDSCAPE_ZSCALE		(1.0f/128.0f)

enum class GRADIENT
{
	WARM,
	COOL,
	COOL_WARM,
	GREY_WHITE,
	GREY_BLACK,
	BLACK_WHITE
};

const TMap<GRADIENT, TArray<FColor>> GRADIENT_data =
{
	TPair<GRADIENT, TArray<FColor>>(GRADIENT::WARM, TArray<FColor>{
		FColor(80, 230, 80, 255),
		FColor(180, 230, 80, 255),
		FColor(230, 230, 80, 255),
		FColor(230, 230, 180, 255),
		FColor(230, 80, 80, 255)	}),

	TPair<GRADIENT, TArray<FColor>>(GRADIENT::COOL, TArray<FColor>{
		FColor(80, 230, 80, 255),
		FColor(80, 230, 180, 255),
		FColor(80, 230, 230, 255),
		FColor(80, 180, 230, 255),
		FColor(80, 80, 230, 255)	}),

	TPair<GRADIENT, TArray<FColor>>(GRADIENT::COOL_WARM, TArray<FColor>{
		FColor(80, 80, 230, 255),
		FColor(80, 180, 230, 255),
		FColor(80, 230, 230, 255),
		FColor(80, 230, 180, 255),
		FColor(80, 230, 80, 255),
		FColor(180, 230, 80, 255),
		FColor(230, 230, 80, 255),
		FColor(230, 180, 80, 255),
		FColor(230, 80, 80, 255)	}),

	TPair<GRADIENT, TArray<FColor>>(GRADIENT::GREY_WHITE, TArray<FColor>{
		FColor(128, 128, 128, 255),
		FColor(192, 192, 192, 255),
		FColor(255, 255, 255, 255)	}),

	TPair<GRADIENT, TArray<FColor>>(GRADIENT::GREY_BLACK, TArray<FColor>{
		FColor(128, 128, 128, 255),
		FColor(64, 64, 64, 255),
		FColor(0, 0, 0, 255)	}),

	TPair<GRADIENT, TArray<FColor>>(GRADIENT::BLACK_WHITE, TArray<FColor>{
		FColor(0, 0, 0, 255),
		FColor(64, 64, 64, 255),
		FColor(128, 128, 128, 255),
		FColor(192, 192, 192, 255),
		FColor(255, 255, 255, 255)	})
};

struct FLandscapeFileResolution
{
	uint32 Width;
	uint32 Height;
};

//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ATerrainTopologyManager : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		UTexture2D* t_output;
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		UTexture2D* t_output_unsmoothed;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		UTexture2D* t_output_smoothed;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		UTexture2D* t_heightmap;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		UTexture2D* t_heightmap_smoothed;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		UTexture2D* t_debug;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		bool m_coloredGradient = false;

	/// <summary>
/// Default mode is no smoothing.
/// </summary>
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		bool b_smoothHeights = false;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		int smoothHeightsCount = 1;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		bool b_smoothOutput = false;

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		int smoothOutputCount = 1;



	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		ALandscape* Landscape;

	ULandscapeInfo* LandscapeInfo;

	FLandscapeFileResolution Resolution_W_H;

	FVector Scale;

protected:

	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		float m_terrainHeight = 512;//Does NOT make sense to me, but ruins slope maps otherwise... Should be MAX UINT...like 65k.
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		int m_width = 0;
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		int m_height = 0;
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		int m_size = 0;
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
		float m_cellLength = 1;
	//UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
	TArray<float> m_heights;

private:
	TArray<FColor> m_posGradient, m_negGradient, m_gradient;

protected:
	bool m_currentColorMode = false;

private:

	UFUNCTION(BlueprintCallable, Category = "Topology", meta = (HidePin = "World"))
		void Start();

	UFUNCTION(BlueprintCallable, Category = "Topology", meta = (HidePin = "World"))
		void Update();

	/// <summary>
	/// Default mode is nothing changes.
	/// </summary>
	/// <returns></returns>
protected:
	virtual bool OnChange();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


	/// <summary>
	/// Create the map. Update to derivered class to implement.
	/// </summary>
	virtual void CreateMap();

	/// <summary>
	/// Load the provided height map.
	/// </summary>
	/// <param name="bigendian"></param>
	/// <returns></returns>
	TArray<float> Load16Bit(bool bigendian = false);

	/// <summary>
	/// Get a hight value ranging from 0 - 1.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	float GetNormalizedHeight(int x, int y, TArray<float>& map);

	/// <summary>
	/// Get a hight value ranging from 0 - actaul height in meters.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	float GetHeight(int x, int y);


	/// <summary>
	/// Get the heigts maps first derivative using Evans-Young method.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	FVector2D* GetFirstDerivative(int x, int y);

	/// <summary>
	/// Get the heigts maps first and second derivative using Evans-Young method.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="d1"></param>
	/// <param name="d2"></param>
	void GetDerivatives(int x, int y, FVector2D*& d1, FVector*& d2);

	/// <summary>
	/// Smooth using a 5X5 Gaussian kernel.
	/// </summary>
	void SmoothMap(TArray<float>& map, int loopCount);

	FLinearColor GetPixel(TArray<FLinearColor> tex, int x, int y, int Width, int Height);

	FLinearColor GetPixelBilinear(TArray<FColor> data, double x, double y, int Width, int Height);

	float FractionalPart(float input);

	/// <summary>
	/// Take a parameter, rescale it and return as a 
	/// color using a gradient. Helps visualize some 
	/// parameters better especially if they have a 
	/// wide dynamic range and can be negative. 
	/// </summary>
	/// <param name="v">The parameter</param>
	/// <param name="exponent">Amount to rescale the dynamic range. 
	/// Will change if terrain cell length changes.</param>
	/// <param name="nonNegative">If the parameter is always positive</param>
	/// <returns></returns>
	FLinearColor Colorize(float v, float exponent, bool nonNegative);

	UTexture2D* CreateTexture(const int32 SrcWidth, const int32 SrcHeight);
	UTexture2D* CreateRGBA8_TextureFromR8_Array(TArray<float>&);

	UTexture2D* TextureFromRAW(const int32 SrcWidth, const int32 SrcHeight, const TArray<FLinearColor>& SrcData,
		const bool UseAlpha, enum TextureFilter sampleMode);


	void CreateGradients(bool colored);

public:

	TArray<uint16> HeightmapFromLandscape();

	bool GetLandscapeExtent(int32& MinX, int32& MinY, int32& MaxX, int32& MaxY) const;

	void GetHeightDataFast(const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, uint16* Data, int32 Stride, uint16* NormalData = NULL, UTexture2D* InHeightmap = nullptr);

	struct FMipInfo
	{
		void* MipData;
		TArray<FUpdateTextureRegion2D> MipUpdateRegions;
		bool bFull;
	};

	//FMipInfo* GetTextureDataInfo(UTexture2D* Texture);

	//ULandscapeInfo* CreateLandscapeInfo(bool bMapCheck);



	template<typename T>
	struct TArrayDataAccess
	{
		typedef T DataType;
		int32 X1;
		int32 Y1;
		DataType* Data;
		int32 Stride;

		TArrayDataAccess(int32 InX1, int32 InY1, DataType* InData, int32 InStride)
			: X1(InX1)
			, Y1(InY1)
			, Data(InData)
			, Stride(InStride)
		{}

		inline void Store(int32 LandscapeX, int32 LandscapeY, DataType NewValue)
		{
			Data[(LandscapeY - Y1) * Stride + (LandscapeX - X1)] = NewValue;
		}

		inline DataType Load(int32 LandscapeX, int32 LandscapeY)
		{
			return Data[(LandscapeY - Y1) * Stride + (LandscapeX - X1)];
		}
	};

	TMap<UTexture2D*, TArray<ULandscapeComponent*>> GenerateComponentsPerHeightmaps() const;

	TMap<FIntPoint, ULandscapeComponent*> GenerateXYtoCompnentMap();

	ULandscapeInfo* CreateLandscapeInfo(bool bMapCheck);


	// Without data interpolation, able to get normal data
	template<typename TStoreData>
	void GetHeightDataTemplFast(const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TStoreData& StoreData, UTexture2D* InHeightmap = nullptr, TStoreData* NormalData = NULL);

	template<typename TStoreData>
	void GetHeightDataTemplFast2(const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TStoreData& StoreData, UTexture2D* InHeightmap = nullptr, TStoreData* NormalData = NULL);




};