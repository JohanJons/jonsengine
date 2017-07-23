#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Resources/Model.h"
#include "include/Resources/TerrainData.h"

#include <string>

namespace JonsEngine
{
	class Terrain
	{
	public:
		Terrain(const std::string& name, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID);

		const std::string& GetName() const;
		SceneNodeID GetSceneNode() const;
		TerrainDataID GetTerrainData() const;


	private:
		std::string mName;
		SceneNodeID mSceneNodeID;
		TerrainDataID mTerrainDataID;
	};

	typedef IDMap<Terrain>::ItemID TerrainID;
	static const TerrainID INVALID_TERRAIN_ID = IDMap<TerrainID>::INVALID_ITEM_ID;
}