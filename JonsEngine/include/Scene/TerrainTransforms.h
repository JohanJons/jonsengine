#pragma once

#include "include/Core/Types.h"
#include "include/Scene/Terrain.h"
#include "include/Scene/SceneNode.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	struct TerrainTransformData
	{
		struct Metadata
		{
			Metadata( TerrainID id, uint32_t endIndex, uint32_t gridSize ) :
				mID(id), mEndIndex(endIndex), mGridSize(gridSize)
			{
			}

			TerrainID mID;
			uint32_t mEndIndex;
			uint32_t mGridSize;
		};

		std::vector<Metadata> mTerrainMetadata;
		std::vector<Mat4> mTransforms;
	};

	class TerrainTransforms
	{
	public:
		TerrainTransforms(const IDMap<Terrain>& terrainLookup, const IDMap<TerrainData>& terrainDataLookup, const IDMapTree<SceneNode>& sceneNodeLookup);

		void AddDirty(TerrainID ID);
		uint32_t UpdateTransforms();

		uint32_t GetNumEntries() const;
		const TerrainTransformData& GetTransformData() const { return mTerrainTransforms; }

	private:
		const IDMap<Terrain>& mTerrainLookup;
		const IDMap<TerrainData>& mTerrainDataLookup;
		const IDMapTree<SceneNode>& mSceneNodeLookup;

		TerrainTransformData mTerrainTransforms;
		std::vector<TerrainID> mDirtyTransforms;
	};
}