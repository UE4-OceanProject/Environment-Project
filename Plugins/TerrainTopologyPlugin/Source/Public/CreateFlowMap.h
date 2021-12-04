#pragma once

#include "TerrainTopologyManager.h"
//#include <vector>
//#include <limits>
//#include "rectangularvectors.h"
#include "CreateFlowMap.generated.h"

//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ACreateFlowMap : public ATerrainTopologyManager
{
	GENERATED_UCLASS_BODY()

private:
	static constexpr int LEFT = 0;
	static constexpr int RIGHT = 1;
	static constexpr int BOTTOM = 2;
	static constexpr int TOP = 3;

	static constexpr float TIME = 0.2f;

public:
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
	int m_iterations = 5;

protected:
	void CreateMap() override;

private:
	void FillWaterMap(float amount, TArray<TArray<float>>& waterMap, int width, int height);

	void ComputeOutflow(TArray<TArray<float>>& waterMap, TArray<TArray<TArray<float>>>& outFlow, TArray<float>& heightMap, int width, int height);

	void UpdateWaterMap(TArray<TArray<float>>& waterMap, TArray<TArray<TArray<float>>>& outFlow, int width, int height);

	void CalculateVelocityField(TArray<TArray<float>>& velocityMap, TArray<TArray<TArray<float>>>& outFlow, int width, int height);

public:
	static void NormalizeMap(TArray<TArray<float>>& map, int width, int height);

};