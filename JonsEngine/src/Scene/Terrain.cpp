#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain(const std::string& name, float heightScale, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID) :
		mName(name),
		mHeightScale(heightScale),
		mSceneNodeID(sceneNodeID),
		mTerrainDataID(terrainDataID)
	{
	}


	float Terrain::GetHeightScale() const
	{
		return mHeightScale;
	}

	void Terrain::SetHeightScale(float heightScale)
	{
		mHeightScale = heightScale;
	}


	const std::string& Terrain::GetName() const
	{
		return mName;
	}

	SceneNodeID Terrain::GetSceneNode() const
	{
		return mSceneNodeID;
	}

	TerrainDataID Terrain::GetTerrainData() const
	{
		return mTerrainDataID;
	}
}