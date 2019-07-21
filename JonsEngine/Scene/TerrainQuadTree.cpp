#include "Scene/TerrainQuadTree.h"

namespace JonsEngine
{
	TerrainQuadTree::TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize ) :
		mPatchMinSize( patchMinSize )
	{
		assert( patchMinSize && width && height && !heightmapData.empty() );

		mGridTraversal.reserve( GetNumNodeElements( width, mPatchMinSize ) );

		AddGridNode();
		QuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB );
	}

	void TerrainQuadTree::GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const
	{
		const QuadNodeAABB& toplevelNode = mGridTraversal.front();
		Vec3 minAABB = toplevelNode.mAABB.Min(), maxAABB = toplevelNode.mAABB.Max();

		worldMin = Vec2( minAABB.x, minAABB.z );
		worldMax = Vec2( maxAABB.x, maxAABB.z );
	}

	uint32_t TerrainQuadTree::GetNumNodeElements( uint32_t width, uint32_t patchMinSize ) const
	{
		assert( width && patchMinSize );
		
		uint32_t curr = width;
		uint32_t pow = 2;
		// +1 for toplevel AABB node
		uint32_t numAABBs = 1;
		while ( curr > patchMinSize )
		{
			numAABBs += 1 << pow;
			curr /= 2;
			pow += 2;
		}

		return numAABBs;
	}

	void TerrainQuadTree::AddGridNode( uint32_t center, uint32_t width, uint32_t height )
	{
		uint32_t nodeWidth = width / 2;
		uint32_t nodeHeight = height / 2;
	}

	void TerrainQuadTree::ProcessQuadNode( QuadNodeAABB& quadAABB )
	{

	}
}
