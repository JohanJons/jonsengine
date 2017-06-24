#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain(const std::string& name, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID) :
		mName(name),
		mSceneNodeID(sceneNodeID),
		mTerrainDataID(terrainDataID)
	{
	}


	const std::string& Terrain::GetName() const
	{
		return mName;
	}

	SceneNodeID Terrain::GetSceneNode() const
	{
		return mSceneNodeID;
	}

	TerrainDataID Terrain::GetTerrainDataID() const
	{
		return mTerrainDataID;
	}
}