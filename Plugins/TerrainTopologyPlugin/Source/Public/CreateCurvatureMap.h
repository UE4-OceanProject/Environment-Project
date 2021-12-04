#pragma once

#include "TerrainTopologyManager.h"
#include "CreateCurvatureMap.generated.h"

UENUM()
enum class CURVATURE_TYPE
{
	PLAN,
	HORIZONTAL,
	VERTICAL,
	MEAN,
	GAUSSIAN,
	MINIMAL,
	MAXIMAL,
	UNSPHERICITY,
	ROTOR,
	DIFFERENCE,
	HORIZONTAL_EXCESS,
	VERTICAL_EXCESS,
	RING,
	ACCUMULATION
};

//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ACreateCurvatureMap : public ATerrainTopologyManager
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Category = "Topology", BlueprintReadWrite, EditAnywhere)
	CURVATURE_TYPE m_curvatureType = CURVATURE_TYPE::MAXIMAL;

private:
	CURVATURE_TYPE m_currentType = static_cast<CURVATURE_TYPE>(0);

protected:
	bool OnChange() override;



	void CreateMap() override;

	/// <summary>
	/// Kp
	/// Plan curvature measures topographic convergence or divergence.
	/// Is positive for diverging flows on ridges and negative converging flows in valleys.
	/// </summary>
private:
	float PlanCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Kh
	/// Same as plan curvature but multiplied by the sine of the slope angle.
	/// Does not take on extremely large values when slope is small.
	/// aka Tangential curvature.
	/// </summary>
	float HorizontalCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Kv
	/// Vertical curvature measures the rate of change of the slope.
	/// Is negative for slope increasing downhill and positive for slope decreasing dowhill.
	/// aka profile curvature.
	/// </summary>
	float VerticalCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// H
	/// Mean curvature represents convergence and relative deceleration with equal weights.
	/// </summary>
	float MeanCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// K
	/// Gaussian curvature retains values in each point on the surface after
	/// its bending without breaking, stretching, and compressing.
	/// </summary>
	float GaussianCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Kmin
	/// Curvature of the principal section with the lowest value.
	/// </summary>
	float MinimalCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// M
	/// Shows the extent to which the shape of the surface is nonspherical.
	/// Non-negative.
	/// </summary>
	float UnsphericityCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Kmax
	/// Curvature of the principal section with the highest value.
	/// </summary>
	float MaximalCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Krot
	/// Flow lines turn clockwise if rot is positive.
	/// </summary>
	float RotorCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// E
	/// Shows what extent the relative deceleration of flows is higher
	/// than the convergence at a given point.
	/// </summary>
	float DifferenceCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Khe
	/// Shows what extent the bending of a normal section tangential to a contour line
	/// is larger that the minimal bending.
	/// Non-negative.
	/// </summary>
	float HorizontalExcessCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Kve
	/// Shows what extent the bending of a normal section having a common tangent line
	/// with a slope line is larger than the minimal bending.
	/// Non-negative.
	/// </summary>
	float VerticalExcessCurvature(float zx, float zy, float zxx, float zyy, float zxy);

	/// <summary>
	/// Kr
	/// Describes flow lines twisting but does not consider direction.
	/// Non-negative.
	/// </summary>
	float RingCurvature(float zx, float zy, float zxx, float zyy, float zxy);
	/// <summary>
	/// Ka
	/// A measure of the extent of the flow accumulation.
	/// </summary>
	float AccumulationCurvature(float zx, float zy, float zxx, float zyy, float zxy);

};