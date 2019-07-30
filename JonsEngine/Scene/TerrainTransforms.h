#pragma once

#include "Core/Types.h"
#include "Core/Containers/FixedGridQuadTree.hpp"
#include "Scene/Terrain.h"
#include "Scene/SceneNode.h"
#include "Scene/TerrainQuadTree.h"

#include <vector>
#include <map>
#include <utility>

namespace JonsEngine
{
	class TerrainTransforms
	{
	public:
		TerrainTransforms(const IDMap<Terrain>& terrainLookup, const IDMap<TerrainData>& terrainDataLookup, const IDMapTree<SceneNode>& sceneNodeLookup);

		void AddDirty(TerrainID ID);
		uint32_t UpdateTransforms( uint32_t patchMinSize );

		const TerrainQuadTree& GetQuadTree( TerrainID ID ) const;

	private:
		void AddAllDirty( uint32_t patchMinSize );
		bool HasAddedDirty( TerrainID ID ) const;
		void RebuildTransforms( TerrainID ID, const Mat4& worldTransform, const Terrain& terrain, const TerrainData& terrainData, uint32_t patchMinSize );

		const IDMap<Terrain>& mTerrainLookup;
		const IDMap<TerrainData>& mTerrainDataLookup;
		const IDMapTree<SceneNode>& mSceneNodeLookup;

		std::map<TerrainID, TerrainQuadTree> mTerrainQuadTreeMap;
		std::vector<TerrainID> mDirtyTransforms;
	};
}