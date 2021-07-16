#pragma once

#include "Topolgy.h"
#include <vector>
#include <cmath>



namespace TerrainTopologyPlugin
{
	class AspectMap : public Topology
	{
	protected:
		bool OnChange() override;

	public:
		std::vector<FVector4*> CreateMap() override;

	private:
		float Aspect(float zx, float zy);

	};

}
