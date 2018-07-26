#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/FixedGridQuadTree.hpp"
#include "include/Scene/Terrain.h"
#include "include/Scene/SceneNode.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	struct TerrainTransformData
	{
		typedef FixedGridQuadTree<Mat4, 32> TerrainTransformGridQuadTree;

		TerrainTransformData( TerrainID ID, std::vector<Mat4>&& transforms, std::vector<Mat4>&& AABBTransforms )
			: mID( ID )
			, mQuadTree( std::move( transforms ), std::move( AABBTransforms ) )
		{ }

		TerrainID mID;
		TerrainTransformGridQuadTree mQuadTree;
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
		void RemoveIfAdded( TerrainID ID );
		bool HasAddedDirty( TerrainID ID ) const;
		void RebuildTransforms( TerrainID ID, const Mat4& worldTransform, const Terrain& terrain, const TerrainData& terrainData );

		const IDMap<Terrain>& mTerrainLookup;
		const IDMap<TerrainData>& mTerrainDataLookup;
		const IDMapTree<SceneNode>& mSceneNodeLookup;

		std::vector<TerrainTransformData> mTerrainTransforms;
		std::vector<TerrainID> mDirtyTransforms;
	};
}