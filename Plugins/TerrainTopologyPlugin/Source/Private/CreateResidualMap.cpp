#include "CreateResidualMap.h"
#include "TMath.h"

ACreateResidualMap::ACreateResidualMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = true;
	b_smoothHeights = false;
}

bool ACreateResidualMap::OnChange()
{
	return m_currentType != m_residualType || m_currentColorMode != m_coloredGradient;
}

void ACreateResidualMap::CreateMap()
{
	m_currentType = m_residualType;

	UTexture2D* residualMap = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(residualMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	auto elevations = TArray<float>();

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			elevations.Empty();

			for (int i = -m_window; i <= m_window; i++)
			{
				for (int j = -m_window; j <= m_window; j++)
				{
					int xi = x + i;
					int yj = y + j;

					if (xi < 0 || xi >= m_width)
					{
						continue;
					}

					if (yj < 0 || yj >= m_height)
					{
						continue;
					}

					float h = GetNormalizedHeight(xi, yj, m_heights);
					elevations.Add(h);
				}
			}

			float residual = 0;
			float h0 = GetNormalizedHeight(x, y, m_heights);
			FLinearColor color = FLinearColor::White;

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

			FColor UEColor = color.ToFColor(false);
			//residualMap->SetPixel(x, y, color);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}

	}

	residualMap->PlatformData->Mips[0].BulkData.Unlock();
	residualMap->UpdateResource();
	//residualMap->Apply();
	t_output = residualMap;
}

float ACreateResidualMap::MeanElevation(TArray<float>& elevations)
{
	return Mean(elevations);
}

float ACreateResidualMap::StdevElevation(TArray<float>& elevations)
{
	auto mean = MeanElevation(elevations);
	return FMath::Sqrt(Variance(elevations, mean));
}

float ACreateResidualMap::DifferenceFromMeanElevation(float h, TArray<float>& elevations)
{
	return h - MeanElevation(elevations);
}

float ACreateResidualMap::DeviationFromMeanElevation(float h, TArray<float>& elevations)
{
	auto o = StdevElevation(elevations);
	auto d = DifferenceFromMeanElevation(h, elevations);

	return static_cast<float>(TMath::SafeDiv(d, o));
}

float ACreateResidualMap::Percentile(float h, TArray<float>& elevations)
{
	int count = elevations.Num();
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

float ACreateResidualMap::Mean(TArray<float>& data)
{
	int count = data.Num();
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

float ACreateResidualMap::Variance(TArray<float>& data, float mean)
{
	int count = data.Num();
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