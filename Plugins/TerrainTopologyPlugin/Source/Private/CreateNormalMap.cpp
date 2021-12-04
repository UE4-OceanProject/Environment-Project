#include "CreateNormalMap.h"

ACreateNormalMap::ACreateNormalMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = false;
	b_smoothHeights = false;
}

void ACreateNormalMap::CreateMap()
{
	UTexture2D* normalMap = CreateTexture(m_width, m_height);

	// Create base mip.
	uint8* DestPtr = static_cast<uint8*>(normalMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1 = GetFirstDerivative(x, y);

			//Not to sure of the orientation.
			//Might need to flip x or y

			auto n = new FVector();
			n->X = d1->X * 0.5f + 0.5f;
			n->Y = -d1->Y * 0.5f + 0.5f;
			n->Z = 1.0f;

			n->Normalize();
			n = new FVector(n->X, n->Y, n->Z);

			//Pixels[x + y * m_width] = FLinearColor(n->X, n->Y , n->Z , 1 );
			FColor UEColor = FLinearColor(n->X, n->Y, n->Z, 1).ToFColor(false);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
			delete n;
		}

	}

	//CreateRGBA8_TextureFromR8_Array(t_output, Pixels);

	
	normalMap->PlatformData->Mips[0].BulkData.Unlock();
	normalMap->UpdateResource();
	//aspectMap->Apply();


	//CreateRGBA8_TextureFromR8_Array(t_output, Pixels);
	t_output = normalMap;

}