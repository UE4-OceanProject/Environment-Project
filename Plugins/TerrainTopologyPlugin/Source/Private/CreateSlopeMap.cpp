#include "CreateSlopeMap.h"
#include "TMath.h"

ACreateSlopeMap::ACreateSlopeMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = false;
	b_smoothHeights = false;
}

bool ACreateSlopeMap::OnChange()
{
	return m_currentColorMode != m_coloredGradient;
}

void ACreateSlopeMap::CreateMap()
{
	UTexture2D* slopeMap = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(slopeMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1 = GetFirstDerivative(x, y);
			float slope = Slope(d1->X, d1->Y);

			auto color = Colorize(slope, 0.4f, true);
			FColor UEColor = color.ToFColor(false);
			//slopeMap->SetPixel(x, y, color);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}
	}

	slopeMap->PlatformData->Mips[0].BulkData.Unlock();
	slopeMap->UpdateResource();
	//slopeMap->Apply();
	t_output = slopeMap;

}

float ACreateSlopeMap::Slope(float zx, float zy)
{
	float p = zx * zx + zy * zy;
	float g = TMath::SafeSqrt(p);

	return FMath::Atan(g) * TMath::Rad2Deg / 90.0f;
}