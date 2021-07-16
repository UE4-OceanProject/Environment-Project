#include "NormalMap.h"



namespace TerrainTopologyPlugin
{

	std::vector<FVector4*> NormalMap::CreateMap()
	{
		std::vector<FVector4*> newmap(getWidth() * getHeight());

		for (int y = 0; y < getHeight(); y++)
		{
			for (int x = 0; x < getWidth(); x++)
			{
				FVector2D *d1 = GetFirstDerivative(x, y);

				//Not to sure of the orientation.
				//Might need to flip x or y

				auto n = new FVector();
				n->X = d1->X * 0.5f + 0.5f;
				n->Y = -d1->Y * 0.5f + 0.5f;
				n->Z = 1.0f;

				n->Normalize();

				newmap[x + y * getWidth()] = new FVector4(n->X, n->Y, n->Z, 1);

//C# TO C++ CONVERTER TODO TASK: A 'delete n' statement was not added since n was passed to a method or constructor. Handle memory management manually.
			}
		}

		return newmap;
	}
}
