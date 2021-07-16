#pragma once

#include "Topolgy.h"
#include <vector>
#include <stdexcept>



namespace TerrainTopologyPlugin
{
	enum class LANDFORM_TYPE
	{
		GAUSSIAN,
		SHAPE_INDEX,
		ACCUMULATION
	};

	class LandformMap : public Topology
	{
	public:
		LANDFORM_TYPE m_landformType = LANDFORM_TYPE::GAUSSIAN;

	private:
		LANDFORM_TYPE m_currentType = static_cast<LANDFORM_TYPE>(0);

	protected:
		bool OnChange() override;

		/// <summary>
		/// Since landforms uses the second derivatives it can be sensitive to noise.
		/// For best results smooth the heights to reduce noise.
		/// </summary>
		/// <returns></returns>
		bool DoSmoothHeights() override;

	public:
		std::vector<FVector4*> CreateMap() override;

		/// <summary>
		/// Ranges from 0 to 1.
		/// Values > 0.5 relate to convex landforms.
		/// Values < 0.5 relate to concave lanforms.
		/// </summary>
	private:
		float GaussianLandform(float zx, float zy, float zxx, float zyy, float zxy);

		/// <summary>
		/// Ranges from 0 to 1.
		/// Values > 0.5 relate to convex landforms.
		/// Values < 0.5 relate to concave lanforms.
		/// Same as Gaussian but on a continual sliding scale.
		/// </summary>
		float ShapeIndexLandform(float zx, float zy, float zxx, float zyy, float zxy);

		/// <summary>
		/// Ranges from 0 to 1.
		/// value 1 where flows dissperse from.
		/// value 0.75 where flow over convex shape.
		/// value 0.5 where flat.
		/// value 0.25 where flow over concave shape.
		/// value 0 where flows accumalate to.
		/// </summary>
		float AccumulationLandform(float zx, float zy, float zxx, float zyy, float zxy);

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


	};

}
