#pragma once

#include "Topolgy.h"
#include <vector>



namespace TerrainTopologyPlugin
{
	class SlopeMap : public Topology
	{
	protected:
		bool OnChange() override;

	public:
		std::vector<FVector4*> CreateMap() override;

	private:
		float Slope(float zx, float zy);
	};
}
