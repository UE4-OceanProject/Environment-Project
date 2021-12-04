#include "CreateLandformMap.h"
#include "TMath.h"

ACreateLandformMap::ACreateLandformMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = true;
	/// <summary>
/// Since landforms uses the second derivatives it can be sensitive to noise.
/// For best results smooth the heights to reduce noise.
/// </summary>
	b_smoothHeights = true;
}

bool ACreateLandformMap::OnChange()
{
	return m_currentType != m_landformType || m_currentColorMode != m_coloredGradient;
}


void ACreateLandformMap::CreateMap()
{
	m_currentType = m_landformType;

	UTexture2D* landformMap = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(landformMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1;
			FVector* d2;
			GetDerivatives(x, y, d1, d2);

			float landform = 0;
			FLinearColor color = FLinearColor::White;

			switch (m_landformType)
			{
			case LANDFORM_TYPE::GAUSSIAN:
				landform = GaussianLandform(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(landform, 0, true);
				break;

			case LANDFORM_TYPE::SHAPE_INDEX:
				landform = ShapeIndexLandform(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(landform, 0, true);
				break;

			case LANDFORM_TYPE::ACCUMULATION:
				landform = AccumulationLandform(d1->X, d1->Y, d2->X, d2->Y, d2->Z);
				color = Colorize(landform, 0, true);
				break;
			};

			FColor UEColor = color.ToFColor(false);
			//landformMap->SetPixel(x, y, color);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}

	}

	landformMap->PlatformData->Mips[0].BulkData.Unlock();
	landformMap->UpdateResource();
	//landformMap->Apply();
	t_output = landformMap;
}

float ACreateLandformMap::GaussianLandform(float zx, float zy, float zxx, float zyy, float zxy)
{
	float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);
	float H = MeanCurvature(zx, zy, zxx, zyy, zxy);

	//Hill (dome)
	if (K > 0 && H > 0)
	{
		return 1;
	}

	//Convex saddle
	if (K < 0 && H > 0)
	{
		return 0.75f;
	}

	//Perfect saddle, Antiform (perfect ridge), Synform (perfect valley), Plane.
	//Should be very rare.
	if (K == 0 || H == 0)
	{
		return 0.5f;
	}

	//Concave saddle
	if (K < 0 && H < 0)
	{
		return 0.25f;
	}

	//Depression (Basin)
	if (K > 0 && H < 0)
	{
		return 0;
	}

	return -1;
}

float ACreateLandformMap::ShapeIndexLandform(float zx, float zy, float zxx, float zyy, float zxy)
{
	float K = GaussianCurvature(zx, zy, zxx, zyy, zxy);
	float H = MeanCurvature(zx, zy, zxx, zyy, zxy);

	float d = TMath::SafeSqrt(H * H - K);

	float si = 2.0f / PI * FMath::Atan(TMath::SafeDiv(H, d));

	return si * 0.5f + 0.5f;
}

float ACreateLandformMap::AccumulationLandform(float zx, float zy, float zxx, float zyy, float zxy)
{
	float Kh = HorizontalCurvature(zx, zy, zxx, zyy, zxy);
	float Kv = VerticalCurvature(zx, zy, zxx, zyy, zxy);

	//Dissipation flows.
	if (Kh > 0 && Kv > 0)
	{
		return 1;
	}

	//Convex transitive.
	if (Kh > 0 && Kv < 0)
	{
		return 0.75f;
	}

	//Planar transitive.
	//Should be very rare.
	if (Kh == 0 || Kv == 0)
	{
		return 0.5f;
	}

	//Concave trasitive.
	if (Kh < 0 && Kv > 0)
	{
		return 0.25f;
	}

	//Accumulative flows.
	if (Kh < 0 && Kv < 0)
	{
		return 0;
	}

	return -1;
}

float ACreateLandformMap::HorizontalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zy2 * zxx - 2.0f * zxy * zx * zy + zx2 * zyy;
	float d = p * FMath::Pow(p + 1, 0.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateLandformMap::VerticalCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zx2 * zxx + 2.0f * zxy * zx * zy + zy2 * zyy;
	float d = p * FMath::Pow(p + 1, 1.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateLandformMap::MeanCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = (1 + zy2) * zxx - 2.0f * zxy * zx * zy + (1 + zx2) * zyy;
	float d = 2 * FMath::Pow(p + 1, 1.5f);

	return TMath::SafeDiv(n, d);
}

float ACreateLandformMap::GaussianCurvature(float zx, float zy, float zxx, float zyy, float zxy)
{
	float zx2 = zx * zx;
	float zy2 = zy * zy;
	float p = zx2 + zy2;

	float n = zxx * zyy - zxy * zxy;
	float d = FMath::Pow(p + 1, 2);

	return TMath::SafeDiv(n, d);
}