#pragma once

#include "Core/Types.h"
#include "Core/Math/AABB.h"

#include <vector>

namespace JonsEngine
{
	struct QuadNodeAABB
	{
		enum ChildOffset : uint32_t
		{
			TOP_LEFT = 0,
			TOP_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT,
			NUM_CHILDREN
		};

		QuadNodeAABB( const uVec2& begin, const uVec2& end ) :
			mBoundsBegin( begin ),
			mBoundsEnd( end )
		{ }

		uVec2 mBoundsBegin;
		uVec2 mBoundsEnd;

		QuadNodeAABB* mChildBegin = nullptr;
	};

	class TerrainQuadTree
	{
	public:
		TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize );

		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		uint32_t GetNumNodeElements( uint32_t width, uint32_t patchMinSize ) const;
		void AddGridNode( uint32_t center, uint32_t width, uint32_t height );
		void ProcessQuadNode( QuadNodeAABB& quadAABB );

		uint32_t mPatchMinSize;
		std::vector<QuadNodeAABB> mGridTraversal;
	};
}
