#include "SlopeMap.h"
#include "FMathTT.h"



namespace TerrainTopologyPlugin
{

	bool SlopeMap::OnChange()
	{
		return m_currentColorMode != m_coloredGradient;
	}

	std::vector<FVector4*> SlopeMap::CreateMap()
	{
		std::vector<FVector4*> newmap(getWidth() * getHeight());
		for (int y = 0; y < getHeight(); y++)
		{
			for (int x = 0; x < getWidth(); x++)
			{
				FVector2D *d1 = GetFirstDerivative(x, y);

				float slope = Slope(d1->X, d1->Y);

				newmap[x + y * getWidth()] = Colorize(slope, 0.4f, true);
			}
		}

		return newmap;
	}

	float SlopeMap::Slope(float zx, float zy)
	{
		float p = zx * zx + zy * zy;
		float g = FMathTT::SafeSqrt(p);

		return FMath::Atan(g) * FMathTT::Rad2Deg / 90.0f;
	}
}
