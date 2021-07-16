#include "ResidualMap.h"
#include "FMathTT.h"



namespace TerrainTopologyPlugin
{

	bool ResidualMap::OnChange()
	{
		return m_currentType != m_residualType || m_currentColorMode != m_coloredGradient;
	}

	std::vector<FVector4*> ResidualMap::CreateMap()
	{
		m_currentType = m_residualType;

		std::vector<FVector4*> newmap(getWidth() * getHeight());

		auto elevations = std::vector<float>();

		for (int y = 0; y < getHeight(); y++)
		{
			for (int x = 0; x < getWidth(); x++)
			{
				elevations.clear();

				for (int i = -m_window; i <= m_window; i++)
				{
					for (int j = -m_window; j <= m_window; j++)
					{
						int xi = x + i;
						int yj = y + j;

						if (xi < 0 || xi >= getWidth())
						{
							continue;
						}
						if (yj < 0 || yj >= getHeight())
						{
							continue;
						}

						float h = GetNormalizedHeight(xi, yj);
						elevations.push_back(h);
					}
				}

				float residual = 0;
				float h0 = GetNormalizedHeight(x, y);
				FVector4 *color = white;

				switch (m_residualType)
				{
					case RESIDUAL_TYPE::ELEVATION:
						residual = h0;
						color = Colorize(residual, 0, true);
						break;

					case RESIDUAL_TYPE::MEAN:
						residual = MeanElevation(elevations);
						color = Colorize(residual, 0, true);
						break;

					case RESIDUAL_TYPE::DIFFERENCE:
						residual = DifferenceFromMeanElevation(h0, elevations);
						color = Colorize(residual, 4, false);
						break;

					case RESIDUAL_TYPE::STDEV:
						residual = DeviationFromMeanElevation(h0, elevations);
						color = Colorize(residual, 0.6f, true);
						break;

					case RESIDUAL_TYPE::DEVIATION:
						residual = DeviationFromMeanElevation(h0, elevations);
						color = Colorize(residual, 0.6f, false);
						break;

					case RESIDUAL_TYPE::PERCENTILE:
						residual = Percentile(h0, elevations);
						color = Colorize(residual, 0.3f, true);
						break;
				}


				newmap[x + y * getWidth()] = color;
			}

		}

		return newmap;
	}

	float ResidualMap::MeanElevation(std::vector<float> &elevations)
	{
		return Mean(elevations);
	}

	float ResidualMap::StdevElevation(std::vector<float> &elevations)
	{
		auto mean = MeanElevation(elevations);
		return FMath::Sqrt(Variance(elevations, mean));
	}

	float ResidualMap::DifferenceFromMeanElevation(float h, std::vector<float> &elevations)
	{
		return h - MeanElevation(elevations);
	}

	float ResidualMap::DeviationFromMeanElevation(float h, std::vector<float> &elevations)
	{
		auto o = StdevElevation(elevations);
		auto d = DifferenceFromMeanElevation(h, elevations);

		return static_cast<float>(FMathTT::SafeDiv(d, o));
	}

	float ResidualMap::Percentile(float h, std::vector<float> &elevations)
	{
		int count = elevations.size();
		float num = 0;

		for (int i = 0; i < count; i++)
		{
			if (elevations[i] < h)
			{
				num++;
			}
		}

		if (num == 0)
		{
			return 0;
		}
		return num / count;
	}

	float ResidualMap::Mean(std::vector<float> &data)
	{
		int count = data.size();
		if (count == 0)
		{
			return 0;
		}

		float u = 0;
		for (int i = 0; i < count; i++)
		{
			u += data[i];
		}

		return u / count;
	}

	float ResidualMap::Variance(std::vector<float> &data, float mean)
	{
		int count = data.size();
		if (count == 0)
		{
			return 0;
		}

		float v = 0;
		for (int i = 0; i < count; i++)
		{
			float diff = data[i] - mean;
			v += diff * diff;
		}

		return v / count;
	}
}
