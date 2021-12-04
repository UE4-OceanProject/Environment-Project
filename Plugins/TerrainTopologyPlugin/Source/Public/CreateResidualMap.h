#pragma once

#include "TerrainTopologyManager.h"
//#include <vector>
#include "CreateResidualMap.generated.h"

UENUM()
enum class RESIDUAL_TYPE
{
	ELEVATION,
	MEAN,
	DIFFERENCE,
	STDEV,
	DEVIATION,
	PERCENTILE
};

//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ACreateResidualMap : public ATerrainTopologyManager
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
	RESIDUAL_TYPE m_residualType = RESIDUAL_TYPE::PERCENTILE;

private:
	int m_window = 3;

	RESIDUAL_TYPE m_currentType = static_cast<RESIDUAL_TYPE>(0);

protected:
	bool OnChange() override;

	void CreateMap() override;

private:
	float MeanElevation(TArray<float>& elevations);

	float StdevElevation(TArray<float>& elevations);

	float DifferenceFromMeanElevation(float h, TArray<float>& elevations);

	float DeviationFromMeanElevation(float h, TArray<float>& elevations);

	float Percentile(float h, TArray<float>& elevations);

	float Mean(TArray<float>& data);

	float Variance(TArray<float>& data, float mean);

};