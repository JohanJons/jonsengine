#include "Scene/TerrainTransforms.h"
#include "Core/Math/MathUtils.h"

#include <algorithm>

namespace JonsEngine
{
	TerrainTransforms::TerrainTransforms(const IDMap<Terrain>& terrainLookup, const IDMap<TerrainData>& terrainDataLookup, const IDMapTree<SceneNode>& sceneNodeLookup) :
		mTerrainLookup(terrainLookup),
		mTerrainDataLookup(terrainDataLookup),
		mSceneNodeLookup(sceneNodeLookup)
	{
	}

	void TerrainTransforms::AddDirty(TerrainID ID)
	{
		auto iter = std::find( mDirtyTransforms.cbegin(), mDirtyTransforms.cend(), ID );
		if ( iter != mDirtyTransforms.cend() )
			return;

		if ( !HasAddedDirty( ID ) )
			mDirtyTransforms.push_back( ID );
	}

	uint32_t TerrainTransforms::UpdateTransforms( uint32_t patchMinSize )
	{
		AddAllDirty( patchMinSize );

		uint32_t numUpdated = 0;
		for (TerrainID ID : mDirtyTransforms)
		{
			const Terrain& terrain = mTerrainLookup.GetItem(ID);
			const TerrainData& terrainData = mTerrainDataLookup.GetItem(terrain.GetTerrainData());
			const Mat4& worldTransform = mSceneNodeLookup.GetNode(terrain.GetSceneNode()).GetWorldTransform();

			mTerrainQuadTreeMap.erase( ID );
			RebuildTransforms( ID, worldTransform, terrain, terrainData, patchMinSize );

			++numUpdated;
		}

		mDirtyTransforms.clear();

		return numUpdated;
	}

	const TerrainQuadTree& TerrainTransforms::GetQuadTree( TerrainID ID ) const
	{
		if ( mTerrainQuadTreeMap.find( ID ) == mTerrainQuadTreeMap.cend() )
		{
			const static TerrainQuadTree gStaticEmptyTree;
			return gStaticEmptyTree;
		}

		return mTerrainQuadTreeMap.at( ID );
	}

	void TerrainTransforms::AddAllDirty( uint32_t patchMinSize )
	{
		for ( const auto& pair : mTerrainQuadTreeMap )
		{
			const TerrainID& ID = pair.first;
			const TerrainQuadTree& tree = pair.second;

			if ( tree.GetPatchMinSize() != patchMinSize )
				AddDirty( ID );
		}
	}

	bool TerrainTransforms::HasAddedDirty( TerrainID ID ) const
	{
		auto beginIter = mDirtyTransforms.cbegin(), endIter = mDirtyTransforms.cend();
		auto iter = std::find( beginIter, endIter, ID );

		return iter != endIter;
	}

	void TerrainTransforms::RebuildTransforms( TerrainID ID, const Mat4& worldTransform, const Terrain& terrain, const TerrainData& terrainData, uint32_t patchMinSize )
	{
		uint32_t heightmapWidth = terrainData.GetWidth(), heightmapHeight = terrainData.GetHeight();
		const std::vector<uint8_t>& heightmapData = terrainData.GetHeightMapData();

		assert( patchMinSize && heightmapWidth && heightmapHeight );

		mTerrainQuadTreeMap.emplace( std::piecewise_construct, std::forward_as_tuple( ID ), std::forward_as_tuple( heightmapData, heightmapWidth, heightmapHeight, patchMinSize, terrain.GetHeightScale(), worldTransform ) );
	}
}
