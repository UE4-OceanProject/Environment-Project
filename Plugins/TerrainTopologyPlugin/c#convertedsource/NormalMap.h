#pragma once

#include "Topolgy.h"
#include <vector>



namespace TerrainTopologyPlugin
{
	class NormalMap : public Topology
	{
	public:
		std::vector<FVector4*> CreateMap() override;

	};
}
