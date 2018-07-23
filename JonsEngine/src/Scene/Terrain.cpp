#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain(const std::string& name, float heightScale, uint32_t patchSize, float heightmapMultiplyer, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID) :
		mName( name ),
		mHeightScale( heightScale ),
		mHeightmapMultiplyer( heightmapMultiplyer ),
		mPatchSize( patchSize ),
		mSceneNodeID( sceneNodeID ),
		mTerrainDataID( terrainDataID )
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

	void Terrain::SetHeightmapMultiplyer( float heightmapMultiplyer )
	{
		mHeightmapMultiplyer = heightmapMultiplyer;
	}
}