#pragma once

#include "Topolgy.h"
#include <vector>
#include <limits>
#include "rectangularvectors.h"



namespace TerrainTopologyPlugin
{
	class FlowMap : public Topology
	{
	private:
		static constexpr int LEFT = 0;
		static constexpr int RIGHT = 1;
		static constexpr int BOTTOM = 2;
		static constexpr int TOP = 3;

		static constexpr float TIME = 0.2f;

	public:
		int m_iterations = 5;

		std::vector<FVector4*> CreateMap() override;

	private:
		void FillWaterMap(float amount, std::vector<std::vector<float>> &waterMap, int width, int height);

		void ComputeOutflow(std::vector<std::vector<float>> &waterMap, std::vector<std::vector<std::vector<float>>> &outFlow, std::vector<float> heightMap, int width, int height);

		void UpdateWaterMap(std::vector<std::vector<float>> &waterMap, std::vector<std::vector<std::vector<float>>> &outFlow, int width, int height);

		void CalculateVelocityField(std::vector<std::vector<float>> &velocityMap, std::vector<std::vector<std::vector<float>>> &outFlow, int width, int height);

	public:
		static void NormalizeMap(std::vector<std::vector<float>> &map, int width, int height);

	};

}
