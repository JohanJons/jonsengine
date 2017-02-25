#pragma once

#include "include/Resources/TerrainData.h"

#include <string>

namespace JonsEngine
{
	class Terrain
	{
	public:
		Terrain(const std::string& name, const TerrainDataID terrainDataID);

		const std::string& GetName() const;
		TerrainDataID GetTerrainDataID() const;


	private:
		std::string mName;
		TerrainDataID mTerrainDataID;
	};

	typedef IDMap<Terrain>::ItemID TerrainID;
	static const TerrainID INVALID_TERRAIN_ID = IDMap<TerrainID>::INVALID_ITEM_ID;
}