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
		typedef std::pair<TerrainID, std::size_t> IDIndexPair;

		std::vector<IDIndexPair> mIDEndIndex;
		std::vector<Mat4> mTransforms;
	};

	class TerrainTransforms
	{
	public:
		TerrainTransforms(const IDMap<Terrain>& terrainLookup, const IDMap<TerrainData>& terrainDataLookup, const IDMapTree<SceneNode>& sceneNodeLookup);

		void AddDirty(TerrainID ID);
		uint32_t UpdateTransforms();

		uint32_t GetNumEntries() const;
		const TerrainTransformData& GetTerrainTransforms() const { return mTerrainTransforms; }

	private:
		const IDMap<Terrain>& mTerrainLookup;
		const IDMap<TerrainData>& mTerrainDataLookup;
		const IDMapTree<SceneNode>& mSceneNodeLookup;

		TerrainTransformData mTerrainTransforms;
		std::vector<TerrainID> mDirtyTransforms;
	};
}