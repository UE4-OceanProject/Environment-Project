#include "FlowMap.h"



namespace TerrainTopologyPlugin
{

	std::vector<FVector4*> FlowMap::CreateMap()
	{
		//C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
		//ORIGINAL LINE: float[,] waterMap = new float[width, height];
		std::vector<std::vector<float>> waterMap = RectangularVectors::RectangularFloatVector(getWidth(), getHeight());
		//C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
		//ORIGINAL LINE: float[,,] outFlow = new float[width, height, 4];
		std::vector<std::vector<std::vector<float>>> outFlow = RectangularVectors::RectangularFloatVector(getWidth(), getHeight(), 4);

		FillWaterMap(0.0001f, waterMap, getWidth(), getHeight());

		for (int i = 0; i < m_iterations; i++)
		{
			ComputeOutflow(waterMap, outFlow, getHeights(), getWidth(), getHeight());
			UpdateWaterMap(waterMap, outFlow, getWidth(), getHeight());
		}

		//C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
		//ORIGINAL LINE: float[,] velocityMap = new float[width, height];
		std::vector<std::vector<float>> velocityMap = RectangularVectors::RectangularFloatVector(getWidth(), getHeight());

		CalculateVelocityField(velocityMap, outFlow, getWidth(), getHeight());
		NormalizeMap(velocityMap, getWidth(), getHeight());

		std::vector<FVector4*> newmap(getWidth() * getHeight());

		for (int y = 0; y < getHeight(); y++)
		{
			for (int x = 0; x < getWidth(); x++)
			{
				float v = velocityMap[x][y];
				newmap[x + y * getWidth()] = new FVector4(v, v, v, 1);
			}
		}

		return newmap;

	}

	void FlowMap::FillWaterMap(float amount, std::vector<std::vector<float>>& waterMap, int width, int height)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				waterMap[x][y] = amount;
			}
		}
	}

	void FlowMap::ComputeOutflow(std::vector<std::vector<float>>& waterMap, std::vector<std::vector<std::vector<float>>>& outFlow, std::vector<float> heightMap, int width, int height)
	{

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int xn1 = (x == 0) ? 0 : x - 1;
				int xp1 = (x == width - 1) ? width - 1 : x + 1;
				int yn1 = (y == 0) ? 0 : y - 1;
				int yp1 = (y == height - 1) ? height - 1 : y + 1;

				float waterHt = waterMap[x][y];
				float waterHts0 = waterMap[xn1][y];
				float waterHts1 = waterMap[xp1][y];
				float waterHts2 = waterMap[x][yn1];
				float waterHts3 = waterMap[x][yp1];

				float landHt = heightMap[x + y * width];
				float landHts0 = heightMap[xn1 + y * width];
				float landHts1 = heightMap[xp1 + y * width];
				float landHts2 = heightMap[x + yn1 * width];
				float landHts3 = heightMap[x + yp1 * width];

				float diff0 = (waterHt + landHt) - (waterHts0 + landHts0);
				float diff1 = (waterHt + landHt) - (waterHts1 + landHts1);
				float diff2 = (waterHt + landHt) - (waterHts2 + landHts2);
				float diff3 = (waterHt + landHt) - (waterHts3 + landHts3);

				//out flow is previous flow plus flow for this time step.
				float flow0 = FMath::Max(0.f, outFlow[x][y][0] + diff0);
				float flow1 = FMath::Max(0.f, outFlow[x][y][1] + diff1);
				float flow2 = FMath::Max(0.f, outFlow[x][y][2] + diff2);
				float flow3 = FMath::Max(0.f, outFlow[x][y][3] + diff3);

				float sum = flow0 + flow1 + flow2 + flow3;

				if (sum > 0.0f)
				{
					//If the sum of the outflow flux exceeds the amount in the cell
					//flow value will be scaled down by a factor K to avoid negative update.
					float K = waterHt / (sum * TIME);
					if (K > 1.0f)
					{
						K = 1.0f;
					}
					if (K < 0.0f)
					{
						K = 0.0f;
					}

					outFlow[x][y][0] = flow0 * K;
					outFlow[x][y][1] = flow1 * K;
					outFlow[x][y][2] = flow2 * K;
					outFlow[x][y][3] = flow3 * K;
				}
				else
				{
					outFlow[x][y][0] = 0.0f;
					outFlow[x][y][1] = 0.0f;
					outFlow[x][y][2] = 0.0f;
					outFlow[x][y][3] = 0.0f;
				}

			}
		}

	}

	void FlowMap::UpdateWaterMap(std::vector<std::vector<float>>& waterMap, std::vector<std::vector<std::vector<float>>>& outFlow, int width, int height)
	{

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				float flowOUT = outFlow[x][y][0] + outFlow[x][y][1] + outFlow[x][y][2] + outFlow[x][y][3];
				float flowIN = 0.0f;

				//Flow in is inflow from neighour cells. Note for the cell on the left you need 
				//thats cells flow to the right (ie it flows into this cell)
				flowIN += (x == 0) ? 0.0f : outFlow[x - 1][y][RIGHT];
				flowIN += (x == width - 1) ? 0.0f : outFlow[x + 1][y][LEFT];
				flowIN += (y == 0) ? 0.0f : outFlow[x][y - 1][TOP];
				flowIN += (y == height - 1) ? 0.0f : outFlow[x][y + 1][BOTTOM];

				float ht = waterMap[x][y] + (flowIN - flowOUT) * TIME;
				if (ht < 0.0f)
				{
					ht = 0.0f;
				}

				//Result is net volume change over time
				waterMap[x][y] = ht;
			}
		}

	}

	void FlowMap::CalculateVelocityField(std::vector<std::vector<float>>& velocityMap, std::vector<std::vector<std::vector<float>>>& outFlow, int width, int height)
	{

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				float dl = (x == 0) ? 0.0f : outFlow[x - 1][y][RIGHT] - outFlow[x][y][LEFT];

				float dr = (x == width - 1) ? 0.0f : outFlow[x][y][RIGHT] - outFlow[x + 1][y][LEFT];

				float dt = (y == height - 1) ? 0.0f : outFlow[x][y + 1][BOTTOM] - outFlow[x][y][TOP];

				float db = (y == 0) ? 0.0f : outFlow[x][y][BOTTOM] - outFlow[x][y - 1][TOP];

				float vx = (dl + dr) * 0.5f;
				float vy = (db + dt) * 0.5f;

				velocityMap[x][y] = FMath::Sqrt(vx * vx + vy * vy);
			}

		}

	}

	void FlowMap::NormalizeMap(std::vector<std::vector<float>>& map, int width, int height)
	{

		float min = std::numeric_limits<float>::infinity();
		float max = -std::numeric_limits<float>::infinity();

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				float v = map[x][y];
				if (v < min)
				{
					min = v;
				}
				if (v > max)
				{
					max = v;
				}
			}
		}

		float size = max - min;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				float v = map[x][y];

				if (size < 1e-12f)
					v = 0;
				else
					v = (v - min) / size;

				map[x][y] = v;
			}
		}

	}
}