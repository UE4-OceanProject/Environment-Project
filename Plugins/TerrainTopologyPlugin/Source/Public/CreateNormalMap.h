#pragma once

#include "TerrainTopologyManager.h"
#include "CreateNormalMap.generated.h"

//An actor based terrain topology generator
//Transient will prevent this from being saved since we autospawn this anyways
//Removed the Transient property, plugin will spawn this if its missing, and wont if its already there
UCLASS()
class TERRAINTOPOLOGYPLUGIN_API ACreateNormalMap : public ATerrainTopologyManager
{

	GENERATED_UCLASS_BODY()

protected:
	void CreateMap() override;

};
