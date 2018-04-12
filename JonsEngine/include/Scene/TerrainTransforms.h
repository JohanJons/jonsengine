#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/GridQuadTree.hpp"
#include "include/Scene/Terrain.h"
#include "include/Scene/SceneNode.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	struct TerrainTransformData
	{
		TerrainTransformData( TerrainID ID )
			: mID( ID )
		{ }

		TerrainID mID;
		TransformGridQuadTree mQuadTree;
	};

	class TerrainTransforms
	{
	public:
		TerrainTransforms(const IDMap<Terrain>& terrainLookup, const IDMap<TerrainData>& terrainDataLookup, const IDMapTree<SceneNode>& sceneNodeLookup);

		void AddDirty(TerrainID ID);
		uint32_t UpdateTransforms();

		uint32_t GetNumEntries() const;
		const std::vector<TerrainTransformData>& GetTransforms() const { return mTerrainTransforms; }

	private:
		bool HasAdded( TerrainID ID ) const;
		TerrainTransformData& GetTransformData( TerrainID ID );

		const IDMap<Terrain>& mTerrainLookup;
		const IDMap<TerrainData>& mTerrainDataLookup;
		const IDMapTree<SceneNode>& mSceneNodeLookup;

		std::vector<TerrainTransformData> mTerrainTransforms;
		std::vector<TerrainID> mDirtyTransforms;
	};
}