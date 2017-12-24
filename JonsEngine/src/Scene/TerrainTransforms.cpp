#include "include/Scene/TerrainTransforms.h"

#include <algorithm>

namespace JonsEngine
{
	namespace
	{
		typedef std::vector<TerrainTransformData::IDIndexPair>::const_iterator IDIndexIterator;
		typedef std::vector<Mat4>::iterator TransformIterator;

		uint32_t GetNumTransforms(const float patchSize, const float width, const float height)
		{
			return width / patchSize * height / patchSize;
		}

		IDIndexIterator AddTransforms(TerrainTransformData& TransformsData, TerrainID ID, const float patchSize, const float width, const float height)
		{
			uint32_t numTransforms = GetNumTransforms(patchSize, width, height);
			TransformsData.mTransforms.insert(TransformsData.mTransforms.cend(), numTransforms, gIdentityMatrix);

			TransformsData.mIDEndIndex.emplace_back(ID, TransformsData.mTransforms.size());

			return TransformsData.mIDEndIndex.cend() - 1;
		}

		void RebuildTransforms(TransformIterator begin, const Mat4& worldTransform, const float patchSize, const float width, const float height)
		{
			assert(patchSize && width && height);
			assert(width == static_cast<uint32_t>(width) && height == static_cast<uint32_t>(height));

			int32_t numWidth = width / patchSize, numHeight = height / patchSize;
			assert(numWidth + numHeight == GetNumTransforms(patchSize, width, height));

			auto iter = begin;
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
		if (iter == mDirtyTransforms.cend())
			return;

		mDirtyTransforms.push_back(ID);
	}

	uint32_t TerrainTransforms::UpdateTransforms()
	{
		uint32_t numUpdated = 0;

		for (TerrainID ID : mDirtyTransforms)
		{
			auto funcIDComparison = [ID](const TerrainTransformData::IDIndexPair& pair) { return ID == pair.first; };

			auto beginIter = mTerrainTransforms.mIDEndIndex.cbegin(), endIter = mTerrainTransforms.mIDEndIndex.cend();
			auto iter = std::find_if(beginIter, endIter, funcIDComparison);
			
			const Terrain& terrain = mTerrainLookup.GetItem(ID);
			const TerrainData& terrainData = mTerrainDataLookup.GetItem(terrain.GetTerrainData());
			float patchSize = terrain.GetPatchSize();
			float width = terrainData.GetWidth(), height = terrainData.GetHeight();
			
			if (iter == endIter)
			{
				iter = AddTransforms(mTerrainTransforms, ID, patchSize, width, height);
				beginIter = mTerrainTransforms.mIDEndIndex.cbegin();
			}

			const Mat4& worldTransform = mSceneNodeLookup.GetNode(terrain.GetSceneNode()).GetWorldTransform();
			std::size_t beginIndex = iter == beginIter ? 0 : (iter - 1)->second;
			auto iterTransformsBegin = mTerrainTransforms.mTransforms.begin();
			RebuildTransforms(iterTransformsBegin + beginIndex, worldTransform, patchSize, width, height);

			++numUpdated;
		}

		return numUpdated;
	}

	uint32_t TerrainTransforms::GetNumEntries() const
	{
		assert(mTerrainTransforms.mTransforms.size() == mTerrainTransforms.mIDEndIndex.size());

		return mTerrainTransforms.mTransforms.size();
	}
}
