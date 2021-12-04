#include "CreateCurvatureMap.h"
#include "TMath.h"

ACreateCurvatureMap::ACreateCurvatureMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = true;
	/// <summary>
/// Since curvature uses the second derivatives it can be sensitive to noise.
/// For best results smooth the heights to reduce noise.
/// </summary>
	b_smoothHeights = true;
}

bool ACreateCurvatureMap::OnChange()
{
	return m_currentType != m_curvatureType || m_currentColorMode != m_coloredGradient;
}


void ACreateCurvatureMap::CreateMap()
{
	m_currentType = m_curvatureType;

	UTexture2D* curveMap = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(curveMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1;
			FVector* d2;
			GetDerivatives(x, y, d1, d2);

			float curvature = 0;
			FLinearColor color;

			switch (m_curvatureType)
			{
			case CURVATURE_TYPE::PLAN:
				curvature = PlanCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 1.5f, false);
				break;

			case CURVATURE_TYPE::HORIZONTAL:
				curvature = HorizontalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.0f, false);
				break;

			case CURVATURE_TYPE::VERTICAL:
				curvature = VerticalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.0f, false);
				break;

			case CURVATURE_TYPE::MEAN:
				curvature = MeanCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.4f, false);
				break;

			case CURVATURE_TYPE::GAUSSIAN:
				curvature = GaussianCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 5.0f, false);
				break;

			case CURVATURE_TYPE::MINIMAL:
				curvature = MinimalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.5f, false);
				break;

			case CURVATURE_TYPE::MAXIMAL:
				curvature = MaximalCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.5f, false);
				break;

			case CURVATURE_TYPE::UNSPHERICITY:
				curvature = UnsphericityCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.0f, true);
				break;

			case CURVATURE_TYPE::ROTOR:
				curvature = RotorCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.5f, false);
				break;

			case CURVATURE_TYPE::DIFFERENCE:
				curvature = DifferenceCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.0f, false);
				break;

			case CURVATURE_TYPE::HORIZONTAL_EXCESS:
				curvature = HorizontalExcessCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.0f, true);
				break;

			case CURVATURE_TYPE::VERTICAL_EXCESS:
				curvature = VerticalExcessCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 2.0f, true);
				break;

			case CURVATURE_TYPE::RING:
				curvature = RingCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 5.0f, true);
				break;

			case CURVATURE_TYPE::ACCUMULATION:
				curvature = AccumulationCurvature(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(curvature, 5.0f, false);
				break;

			default:
				color = FLinearColor::White;
				break;
			};
			FColor UEColor = color.ToFColor(false);

			//curveMap->SetPixel(x, y, color);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}

	}

	curveMap->PlatformData->Mips[0].BulkData.Unlock();
	curveMap->UpdateResource();
	//curveMap->Apply();
	t_output = curveMap;
}

float ACreateCurvatureMap::PlanCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zy2 * zxx - 2.0f * zxy * zx * zy + zx2 * zyy;
	float d = FMath::Pow(p, 1.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateCurvatureMap::HorizontalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zy2 * zxx - 2.0f * zxy * zx * zy + zx2 * zyy;
	float d = p * FMath::Pow(p + 1, 0.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateCurvatureMap::VerticalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zx2 * zxx + 2.0f * zxy * zx * zy + zy2 * zyy;
	float d = p * FMath::Pow(p + 1, 1.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateCurvatureMap::MeanCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = (1 + zy2) * zxx - 2.0f * zxy * zx * zy + (1 + zx2) * zyy;
	float d = 2 * FMath::Pow(p + 1, 1.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateCurvatureMap::GaussianCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zxx * zyy - zxy * zxy;
	float d = FMath::Pow(p + 1, 2.0f);

	return TMath::SafeDiv(n, d);
}

float ACreateCurvatureMap::MinimalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
	float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);

	return H - TMath::SafeSqrt(H * H - K);
}

float ACreateCurvatureMap::UnsphericityCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
	float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);

	return TMath::SafeSqrt(H * H - K);
}

float ACreateCurvatureMap::MaximalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
	float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);

	return H + TMath::SafeSqrt(H * H - K);
}

float ACreateCurvatureMap::RotorCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = (zx2 - zy2) * zxy - zx * zy * (zxx - zyy);
	float d = FMath::Pow(p, 1.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateCurvatureMap::DifferenceCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);
	float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);

	return 0.5f * (Kv - Kh);
}

float ACreateCurvatureMap::HorizontalExcessCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);
	float Kmin = MinimalCurvature(zx, zy, zxx, zyy, zxy);

	return Kh - Kmin;
}

float ACreateCurvatureMap::VerticalExcessCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);
	float Kmin = MinimalCurvature(zx, zy, zxx, zyy, zxy);

	return Kv - Kmin;
}

float ACreateCurvatureMap::RingCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float H = MeanCurvature(zx, zy, zxx, zyy, zxy);
	float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);
	float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);

	return 2 * H * Kh - Kh * Kh - K;
}

float ACreateCurvatureMap::AccumulationCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);
	float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);

	return Kh * Kv;
}
