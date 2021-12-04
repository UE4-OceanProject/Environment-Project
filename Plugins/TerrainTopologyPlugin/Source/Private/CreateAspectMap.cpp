#include "CreateAspectMap.h"
#include "TMath.h"

ACreateAspectMap::ACreateAspectMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = true;
	b_smoothHeights = false;
}

bool ACreateAspectMap::OnChange()
{
	return m_currentColorMode != m_coloredGradient;
}

void ACreateAspectMap::CreateMap()
{
	UTexture2D* aspectMap = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(aspectMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1 = GetFirstDerivative(x, y);
			float aspect = static_cast<float>(Aspect(d1->X, d1->Y));

			auto UEColor = Colorize(aspect, 0, true).ToFColor(false);
			//aspectMap->SetPixel(x, y, color);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}

	}
	
	aspectMap->PlatformData->Mips[0].BulkData.Unlock();
	aspectMap->UpdateResource();
	//aspectMap->Apply();

	t_output = aspectMap;
}

float ACreateAspectMap::Aspect(float zx, float zy)
{
	float gyx = TMath::SafeDiv(zy, zx);
	float gxx = TMath::SafeDiv(zx, FMath::Abs(zx));

	float aspect = 180 - FMath::Atan(gyx) * TMath::Rad2Deg + 90 * gxx;
	aspect /= 360;

	return aspect;
}