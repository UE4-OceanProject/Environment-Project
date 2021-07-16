#include "AspectMap.h"
#include "FMathTT.h"



namespace TerrainTopologyPlugin
{

	bool AspectMap::OnChange()
	{
		return m_currentColorMode != m_coloredGradient;
	}

	std::vector<FVector4*> AspectMap::CreateMap()
	{

		std::vector<FVector4*> newmap(getWidth() * getHeight());

		for (int y = 0; y < getHeight(); y++)
		{
			for (int x = 0; x < getWidth(); x++)
			{
				FVector2D *d1 = GetFirstDerivative(x, y);

				float aspect = static_cast<float>(Aspect(d1->X, d1->Y));

				newmap[x + y * getWidth()] = Colorize(aspect, 0, true);
			}

		}
		return newmap;
	}

	float AspectMap::Aspect(float zx, float zy)
	{
		float gyx = FMathTT::SafeDiv(zy, zx);
		float gxx = FMathTT::SafeDiv(zx, std::abs(zx));

		float aspect = 180 - FMath::Atan(gyx) * FMathTT::Rad2Deg + 90 * gxx;
		aspect /= 360;

		return aspect;
	}
}
