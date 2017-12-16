#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain(const std::string& name, float heightScale, uint32_t patchSize, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID) :
		mName(name),
		mHeightScale(heightScale),
		mPatchSize(patchSize),
		mSceneNodeID(sceneNodeID),
		mTerrainDataID(terrainDataID)
	{
	}


	void Terrain::SetHeightScale(float heightScale)
	{
		mHeightScale = heightScale;
	}

	void Terrain::SetPatchSize(uint32_t patchSize)
	{
		mPatchSize = patchSize;
	}
}