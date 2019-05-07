#include "Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain( const std::string& name, float heightScale, float variationScale, uint32_t patchSize, float terrainSizeMultiplyer, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID, const OnTerrainDirtyFunc& onDirtyFunc ) :
		mName( name ),
		mHeightScale( heightScale ),
		mVariationScale( variationScale ),
		mTerrainSizeMultiplyer( terrainSizeMultiplyer ),
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

	void Terrain::SetVariationScale( float variationScale )
	{
		mVariationScale = variationScale;

		// no need to dirty for this
	}

	void Terrain::SetPatchSize( uint32_t patchSize )
	{
		mPatchSize = patchSize;

		mOnDirtyFunc( this );
	}

	void Terrain::SetTerrainSizeMultiplyer( float terrainSizeMultiplyer )
	{
		mTerrainSizeMultiplyer = terrainSizeMultiplyer;

		mOnDirtyFunc( this );
	}


	void Terrain::SetCallback( const OnTerrainDirtyFunc& onDirty )
	{
		mOnDirtyFunc = onDirty;
	}
}