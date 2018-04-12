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

		void RebuildTransforms( TerrainTransformData& TransformData, const Mat4& worldTransform, const float patchSize, const float width, const float height)
		{
			assert(patchSize && width && height);
			assert(width == static_cast<uint32_t>(width) && height == static_cast<uint32_t>(height));

			int32_t numWidth = GetNumColumns( patchSize, width ), numHeight = GetNumRows(patchSize, height);
			int32_t gridSize = numWidth * numHeight;

			std::vector<Mat4>& GridNodes = TransformData.mQuadTree.GetNodes();
			GridNodes.resize( gridSize );

			auto iter = GridNodes.begin();
			float patchExtent = patchSize / 2;
			Mat4 patchScaleTransform = glm::scale(Vec3(patchExtent, 1.0f, patchExtent));
			for (int32_t colNum = -numHeight; colNum < numHeight; colNum += 2)
			{
				for (int32_t rowNum = -numWidth; rowNum < numWidth; rowNum += 2, ++iter)
				{
					Mat4& transform = *iter;
					transform = worldTransform * patchScaleTransform * glm::translate(Vec3(rowNum, 0.0f, colNum));
					transform = glm::transpose(transform);
				}
			}
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

		if ( !HasAdded( ID ) )
			mTerrainTransforms.emplace_back( ID );

		mDirtyTransforms.push_back(ID);
	}

	uint32_t TerrainTransforms::UpdateTransforms()
	{
		uint32_t numUpdated = 0;

		for (TerrainID ID : mDirtyTransforms)
		{
			const Terrain& terrain = mTerrainLookup.GetItem(ID);
			const TerrainData& terrainData = mTerrainDataLookup.GetItem(terrain.GetTerrainData());
			float patchSize = terrain.GetPatchSize();
			float width = terrainData.GetWidth(), height = terrainData.GetHeight();

			const Mat4& worldTransform = mSceneNodeLookup.GetNode(terrain.GetSceneNode()).GetWorldTransform();

			TerrainTransformData& TransformData = GetTransformData( ID );
			RebuildTransforms( TransformData, worldTransform, patchSize, width, height);

			++numUpdated;
		}

		return numUpdated;
	}

	uint32_t TerrainTransforms::GetNumEntries() const
	{
		return mTerrainTransforms.size();
	}

	bool TerrainTransforms::HasAdded( TerrainID ID ) const
	{
		auto funcIDComparison = [ ID ]( const TerrainTransformData& metadata ) { return ID == metadata.mID; };
		auto beginIter = mTerrainTransforms.cbegin(), endIter = mTerrainTransforms.cend();
		auto iter = std::find_if( beginIter, endIter, funcIDComparison );

		return iter != endIter;
	}

	TerrainTransformData& TerrainTransforms::GetTransformData( TerrainID ID )
	{
		auto funcIDComparison = [ ID ]( const TerrainTransformData& metadata ) { return ID == metadata.mID; };
		auto beginIter = mTerrainTransforms.begin(), endIter = mTerrainTransforms.end();
		auto iter = std::find_if( beginIter, endIter, funcIDComparison );
		assert( iter != endIter );

		return *iter;
	}
}
