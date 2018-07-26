#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain( const std::string& name, float heightScale, uint32_t patchSize, float heightmapMultiplyer, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID, const OnTerrainDirtyFunc& onDirtyFunc ) :
		mName( name ),
		mHeightScale( heightScale ),
		mHeightmapMultiplyer( heightmapMultiplyer ),
		mPatchSize( patchSize ),
		mSceneNodeID( sceneNodeID ),
		mTerrainDataID( terrainDataID ),
		mOnDirtyFunc( onDirtyFunc )
	{
	}


	void Terrain::SetHeightScale( float heightScale )
	{
		mHeightScale = heightScale;

		mOnDirtyFunc( this );
	}

	void Terrain::SetPatchSize( uint32_t patchSize )
	{
		mPatchSize = patchSize;

		mOnDirtyFunc( this );
	}

	void Terrain::SetHeightmapMultiplyer( float heightmapMultiplyer )
	{
		mHeightmapMultiplyer = heightmapMultiplyer;

		mOnDirtyFunc( this );
	}
}