#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain(const std::string& name, const TerrainDataID terrainDataID) :
		mName(name),
		mTerrainDataID(terrainDataID)
	{
	}


	const std::string& Terrain::GetName() const
	{
		return mName;
	}

	TerrainDataID Terrain::GetTerrainDataID() const
	{
		return mTerrainDataID;
	}
}