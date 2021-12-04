#pragma once

#include "TerrainTopologyManager.h"
#include "CreateSlopeMap.generated.h"


//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ACreateSlopeMap : public ATerrainTopologyManager
{
	GENERATED_UCLASS_BODY()

protected:
	bool OnChange() override;

	void CreateMap() override;

private:
	float Slope(float zx, float zy);

};