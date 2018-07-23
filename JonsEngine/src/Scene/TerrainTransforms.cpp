#include "include/Scene/TerrainTransforms.h"

#include <algorithm>

namespace JonsEngine
{
	namespace
	{
		int32_t GetNumColumns( const float patchSize, const float width )
		{
			return width / patchSize;
		}

		int32_t GetNumRows( const float patchSize, const float height )
		{
			return height / patchSize;
		}
	}

	TerrainTransforms::TerrainTransforms(const IDMap<Terrain>& terrainLookup, const IDMap<TerrainData>& terrainDataLookup, const IDMapTree<SceneNode>& sceneNodeLookup) :
		mTerrainLookup(terrainLookup),
		mTerrainDataLookup(terrainDataLookup),
		mSceneNodeLookup(sceneNodeLookup)
	{
	}

	void TerrainTransforms::AddDirty(TerrainID ID)
	{
		auto iter = std::find(mDirtyTransforms.cbegin(), mDirtyTransforms.cend(), ID);
		if (iter != mDirtyTransforms.cend())
			return;

		if ( !HasAddedDirty( ID ) )
			mDirtyTransforms.push_back( ID );
	}

	uint32_t TerrainTransforms::UpdateTransforms()
	{
		uint32_t numUpdated = 0;

		for (TerrainID ID : mDirtyTransforms)
		{
			const Terrain& terrain = mTerrainLookup.GetItem(ID);
			const TerrainData& terrainData = mTerrainDataLookup.GetItem(terrain.GetTerrainData());
			float patchSize = terrain.GetPatchSize();
			float width = terrainData.GetWidth() / terrain.GetHeightmapMultiplyer(), height = terrainData.GetHeight() / terrain.GetHeightmapMultiplyer();

			const Mat4& worldTransform = mSceneNodeLookup.GetNode(terrain.GetSceneNode()).GetWorldTransform();

			RemoveIfAdded( ID );
			RebuildTransforms( ID, worldTransform, patchSize, width, height );

			++numUpdated;
		}

		mDirtyTransforms.clear();

		return numUpdated;
	}

	uint32_t TerrainTransforms::GetNumEntries() const
	{
		return mTerrainTransforms.size();
	}

	void TerrainTransforms::RemoveIfAdded( TerrainID ID )
	{
		auto funcIDComparison = [ ID ]( const TerrainTransformData& metadata ) { return ID == metadata.mID; };
		auto beginIter = mTerrainTransforms.begin(), endIter = mTerrainTransforms.end();
		std::remove_if( beginIter, endIter, funcIDComparison );
	}

	bool TerrainTransforms::HasAddedDirty( TerrainID ID ) const
	{
		auto beginIter = mDirtyTransforms.cbegin(), endIter = mDirtyTransforms.cend();
		auto iter = std::find( beginIter, endIter, ID );

		return iter != endIter;
	}

	void TerrainTransforms::RebuildTransforms( TerrainID ID, const Mat4& worldTransform, const float patchSize, const float width, const float height )
	{
		assert( patchSize && width && height );
		assert( width == static_cast<uint32_t>( width ) && height == static_cast<uint32_t>( height ) );

		int32_t numWidth = GetNumColumns( patchSize, width ), numHeight = GetNumRows( patchSize, height );
		int32_t gridSize = numWidth * numHeight;

		std::vector<Mat4> transforms;
		std::vector<AABB> AABBs;
		transforms.reserve( gridSize );
		AABBs.reserve( gridSize );

		float patchExtent = patchSize / 2;
		Mat4 patchScaleTransform = glm::scale( Vec3( patchExtent, 1.0f, patchExtent ) );
		for ( int32_t colNum = -numHeight; colNum < numHeight; colNum += 2 )
		{
			for ( int32_t rowNum = -numWidth; rowNum < numWidth; rowNum += 2 )
			{
				transforms.emplace_back( worldTransform * patchScaleTransform * glm::translate( Vec3( rowNum, 0.0f, colNum ) ) );
				AABBs.emplace_back( AABB::gUnitAABB * transforms.back() );
			}
		}

		mTerrainTransforms.emplace_back( ID, std::move( transforms ), std::move( AABBs ) );
	}
}
