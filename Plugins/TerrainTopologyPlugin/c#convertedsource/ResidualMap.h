#pragma once

#include "Topolgy.h"
#include <vector>



namespace TerrainTopologyPlugin
{
	enum class RESIDUAL_TYPE
	{
		ELEVATION,
		MEAN,
		DIFFERENCE,
		STDEV,
		DEVIATION,
		PERCENTILE
	};

	class ResidualMap : public Topology
	{

	public:
		RESIDUAL_TYPE m_residualType = RESIDUAL_TYPE::PERCENTILE;

	private:
		int m_window = 3;

		RESIDUAL_TYPE m_currentType = static_cast<RESIDUAL_TYPE>(0);

	protected:
		bool OnChange() override;

	public:
		std::vector<FVector4*> CreateMap() override;

	private:
		float MeanElevation(std::vector<float> &elevations);

		float StdevElevation(std::vector<float> &elevations);

		float DifferenceFromMeanElevation(float h, std::vector<float> &elevations);

		float DeviationFromMeanElevation(float h, std::vector<float> &elevations);

		float Percentile(float h, std::vector<float> &elevations);

		float Mean(std::vector<float> &data);

		float Variance(std::vector<float> &data, float mean);

	};

}
