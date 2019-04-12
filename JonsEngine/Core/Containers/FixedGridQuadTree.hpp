#pragma once

#include <vector>
#include <array>

#include "Types.h"
#include "Math.h"
#include "AABB.h"
#include "RenderQueueTypes.h"

namespace JonsEngine
{
	struct GridQuadNodeAABB
	{
		enum ChildOffset : uint32_t
		{
			TOP_LEFT = 0,
			TOP_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT,
			NUM_CHILDREN
		};

		GridQuadNodeAABB( const Vec3& minAABB, const Vec3& maxAABB, uint16_t beginIndex, uint16_t endIndex ) :
			mAABB( minAABB, maxAABB ),
			mBeginIndex( beginIndex ),
			mEndIndex( endIndex )
		{ }

		bool IsValid() const { return mEndIndex >= mBeginIndex; }

		AABB mAABB;
		GridQuadNodeAABB* mChildBegin = nullptr;
		// endIndex inclusive
		uint16_t mBeginIndex = 0, mEndIndex = 0;
	};

	template<typename Item, uint32_t gNodeAABBCutoffPoint>
	class FixedGridQuadTree
	{
	public:
		// Items and AABBs must be sorted left-to-right, row-major in memory, uniform in size
		FixedGridQuadTree( std::vector<Item>&& Items, std::vector<Mat4>&& AABBTransforms );

		uint32_t GetGridSize() const { return mNodes.size(); }

		Item& GetNode( uint32_t index ) { return mNodes.at( index ); }
		const Item& GetNode( uint32_t index ) const { return mNodes.at( index ); }

		// AABBs are in worldspace already hence we only need VP transform
		void CullNodes( std::vector<Item>& nodes, const Mat4& cameraViewProjTransform ) const;
		void CullAABBs( std::vector<Mat4>& aabbTransforms, const Mat4& cameraViewProjTransform ) const;
		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		void CullQuad( std::vector<uint32_t>& nodes, const GridQuadNodeAABB& quadAABB, const Mat4& cameraViewProjTransform ) const;
		void AddAllItems( std::vector<uint32_t>& nodes, const GridQuadNodeAABB& quadAABB ) const;

		void BuildAABBTraversal();
		void GetAABBBounds( Vec3& min, Vec3& max, uint32_t beginIndex, uint32_t endIndex ) const;
		void ProcessQuadNode( GridQuadNodeAABB& quadAABB );
		void AddGridNode( uint32_t beginIndex, uint32_t endIndex );
		bool VerifyMemoryLayout() const;
		bool CheckUniformSize() const;
		bool CheckAABBBoundaries() const;

		uint32_t GetIndex( uint32_t rowIndex, uint32_t columnIndex ) const;
		uint32_t GetColumn( uint32_t index ) const;
		uint32_t GetRow( uint32_t index ) const;
		uint32_t GetNumColumns() const;
		uint32_t GetNumRows( uint32_t numCols ) const;
		uint32_t GetNumNodeElements() const;
		AABB GetAABB( uint32_t index ) const;

		std::vector<Item> mNodes;
		std::vector<Mat4> mNodeAABBs;

		uint32_t mNumColumns;
		uint32_t mNumRows;

		std::vector<GridQuadNodeAABB> mGridTraversal;
	};


	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::FixedGridQuadTree( std::vector<Item>&& Items, std::vector<Mat4>&& AABBTransforms )
		: mNodes( Items )
		, mNodeAABBs( AABBTransforms )
		, mNumColumns( GetNumColumns() )
		, mNumRows( GetNumRows( mNumColumns ) )
	{
		BuildAABBTraversal();
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CullNodes( std::vector<Item>& nodes, const Mat4& cameraViewProjTransform ) const
	{
		assert( !mGridTraversal.empty() );
		std::vector<uint32_t> culledIndices;
		culledIndices.reserve( mNumColumns );	// some reasonable number of items

		CullQuad( culledIndices, mGridTraversal.front(), cameraViewProjTransform );
		for ( uint32_t index : culledIndices )
			nodes.emplace_back( mNodes.at( index ) );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CullAABBs( std::vector<Mat4>& aabbTransforms, const Mat4& cameraViewProjTransform ) const
	{
		assert( mNodeAABBs.size() == mNodes.size() );
		std::vector<uint32_t> culledIndices;
		culledIndices.reserve( mNumColumns );	// some reasonable number of items

		CullQuad( culledIndices, mGridTraversal.front(), cameraViewProjTransform );
		for ( uint32_t index : culledIndices )
			aabbTransforms.emplace_back( mNodeAABBs.at( index ) );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const
	{
		const GridQuadNodeAABB& toplevelNode = mGridTraversal.front();
		Vec3 minAABB = toplevelNode.mAABB.Min(), maxAABB = toplevelNode.mAABB.Max();

		worldMin = Vec2( minAABB.x, minAABB.z );
		worldMax = Vec2( maxAABB.x, maxAABB.z );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CullQuad( std::vector<uint32_t>& nodes, const GridQuadNodeAABB& quadAABB, const Mat4& cameraViewProjTransform ) const
	{
		AABBIntersection intersection = Intersection( quadAABB.mAABB, cameraViewProjTransform );
		switch ( intersection )
		{
			case AABBIntersection::Partial:
			{
				if ( !quadAABB.mChildBegin )
					AddAllItems( nodes, quadAABB );
				else
				{
					for ( uint32_t childIndex = GridQuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < GridQuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
					{
						const GridQuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
						if ( !childQuad.IsValid() )
							continue;

						CullQuad( nodes, childQuad, cameraViewProjTransform );
					}
				}
				break;
			}
			case AABBIntersection::Inside:
			{
				AddAllItems( nodes, quadAABB );
				break;
			}
			default:
				break;
		}
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::AddAllItems( std::vector<uint32_t>& nodes, const GridQuadNodeAABB& quadAABB ) const
	{
		if ( !quadAABB.IsValid() )
			return;

		uint32_t begin = quadAABB.mBeginIndex, end = quadAABB.mEndIndex;
		uint32_t beginRow = GetRow( begin ), endRow = GetRow( end );
		uint32_t beginCol = GetColumn( begin ), endCol = GetColumn( end );
		assert( beginRow >= 0 && endRow >= beginRow && endRow < mNumRows );
		assert( beginCol >= 0 && endCol >= beginCol && endCol < mNumColumns );

		// endRow/endColumn are inclusive indices
		for ( uint32_t i = beginRow; i <= endRow; ++i )
		{
			uint32_t rowBeginIndex = GetIndex( i, beginCol ), rowEndIndex = GetIndex( i, endCol );
			while ( rowBeginIndex <= rowEndIndex )
				nodes.push_back( rowBeginIndex++ );
		}
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::BuildAABBTraversal()
	{
		assert( mNodes.size() == mNodeAABBs.size() );
		assert( VerifyMemoryLayout() );

		mGridTraversal.reserve( GetNumNodeElements() );

		uint32_t beginIndex = 0, endIndex = mNodes.size() - 1;
		AddGridNode( beginIndex, endIndex );

		GridQuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetAABBBounds( Vec3& min, Vec3& max, uint32_t beginIndex, uint32_t endIndex ) const
	{
		if ( beginIndex > endIndex )
			return;

		uint32_t beginRow = GetRow( beginIndex ), endRow = GetRow( endIndex );
		uint32_t beginCol = GetColumn( beginIndex ), endCol = GetColumn( endIndex );
		assert( beginRow >= 0 && endRow >= beginRow && endRow < mNumRows );
		assert( beginCol >= 0 && endCol >= beginCol && endCol < mNumColumns );

		float inf = std::numeric_limits<float>::infinity();
		min = Vec3( inf, inf, inf ), max = Vec3( -inf, -inf, -inf );
		for ( uint32_t row = beginRow; row <= endRow; ++row )
		{
			for ( uint32_t col = beginCol; col <= endCol; ++col )
			{
				uint32_t index = ( row * mNumColumns ) + col;
				const AABB aabb = GetAABB( index );
				
				min = MinVal( min, aabb.Min() );
				max = MaxVal( max, aabb.Max() );
			}
		}
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::ProcessQuadNode( GridQuadNodeAABB& quadAABB )
	{
		if ( !quadAABB.IsValid() || ( quadAABB.mAABB.GetExtent().x <= static_cast<float>( gNodeAABBCutoffPoint ) ) )
			return;

		const Vec3 splitExtent( quadAABB.mAABB.GetExtent().x, 0, 0 );
		const Vec3 tlAABBMin = quadAABB.mAABB.Min(), brAABBMax = quadAABB.mAABB.Max();
		const Vec3 center = quadAABB.mAABB.GetCenter();

		const uint16_t parentBegin = quadAABB.mBeginIndex, parentEnd = quadAABB.mEndIndex;

		uint32_t beginColumn = GetColumn( parentBegin ), beginRow = GetRow( parentBegin );
		uint32_t endColumn = GetColumn( parentEnd ), endRow = GetRow( parentEnd );
		assert( endColumn >= beginColumn && endRow >= beginRow );

		uint32_t numCols = endColumn - beginColumn, numRows = endRow - beginRow;

		AddGridNode( GetIndex( beginRow, beginColumn ), GetIndex( beginRow + numRows / 2, beginColumn + numCols / 2 ) );
		quadAABB.mChildBegin = &mGridTraversal.back();
		AddGridNode( GetIndex( beginRow, ( beginColumn + numCols / 2 ) + 1 ), GetIndex( beginRow + numRows / 2, endColumn ) );
		AddGridNode( GetIndex( ( beginRow + numRows / 2 ) + 1, beginColumn ), GetIndex( endRow, beginColumn + numCols / 2 ) );
		AddGridNode( GetIndex( ( beginRow + numRows / 2 ) + 1, ( beginColumn + numCols / 2 ) + 1 ), GetIndex( endRow, endColumn ) );

		for ( uint32_t offset = GridQuadNodeAABB::TOP_LEFT; offset < GridQuadNodeAABB::NUM_CHILDREN; ++offset )
			ProcessQuadNode( *( quadAABB.mChildBegin + offset ) );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::AddGridNode( uint32_t beginIndex, uint32_t endIndex )
	{
		Vec3 min, max;
		GetAABBBounds( min, max, beginIndex, endIndex );

		mGridTraversal.emplace_back( min, max, beginIndex, endIndex );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	bool FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::VerifyMemoryLayout() const
	{
		if ( mNodeAABBs.size() <= 0 )
		{
			return true;
		}
		
		// some unnecessary re-traversal inflicted
		CheckUniformSize();
		assert( mNumColumns > 0 );
		if ( mNodeAABBs.size() % mNumColumns != 0 )
			return false;

		if ( mNumColumns * mNumRows != mNodeAABBs.size() )
			return false;

		return CheckAABBBoundaries();
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	bool FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CheckUniformSize() const
	{
		assert( !mNodeAABBs.empty() );

		const AABB prevAABB = GetAABB( 0 );
		for ( uint32_t index = 1, numNodes = mNodes.size(); index < numNodes; ++index )
		{
			const AABB currAABB = GetAABB( index );

			if ( prevAABB.GetExtent() != currAABB.GetExtent() )
				return false;
		}

		return true;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	bool FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CheckAABBBoundaries() const
	{
		assert( mNumColumns >= 1 );

		for ( uint32_t index = 0, numNodes = mNodes.size() - mNumColumns; index < numNodes; ++index )
		{
			const AABB currAABB = GetAABB( index );
			const AABB nextAABB = GetAABB( index + 1 );
			const AABB bottomAABB = GetAABB( index + mNumColumns );

			Vec3 currMin = currAABB.Min(), currMax = currAABB.Max();
			Vec3 nextMin = nextAABB.Min();
			Vec3 bottomMin = bottomAABB.Min();

			bool bIsNotAtEndColumn = ( index + 1 ) % mNumColumns != 0;
			if ( bIsNotAtEndColumn && currMax.x != nextMin.x )
				return false;

			if ( currMax.z != bottomMin.z )
				return false;
		}

		return true;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetIndex( uint32_t rowIndex, uint32_t columnIndex ) const
	{
		uint32_t index = columnIndex + ( rowIndex * mNumColumns );

		return index;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetColumn( uint32_t index ) const
	{
		return index % mNumColumns;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetRow( uint32_t index ) const
	{
		return index / mNumColumns;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetNumColumns() const
	{
		assert( !mNodeAABBs.empty() );

		uint32_t numCols = 1;
		AABB prevAABB = GetAABB( 0 );
		for ( uint32_t index = 1, numNodes = mNodes.size(); index < numNodes; ++index )
		{
			const AABB currAABB = GetAABB( index );

			Vec3 currCenter = currAABB.GetCenter(), currExtent = currAABB.GetExtent();
			Vec3 prevCenter = prevAABB.GetCenter(), prevExtent = prevAABB.GetExtent();

			// row-jump
			if ( currCenter.z != prevCenter.z )
			{
				break;
			}

			assert( currCenter.x != prevCenter.x );
			++numCols;

			prevAABB = currAABB;
		}

		return numCols;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetNumRows( uint32_t numCols ) const
	{
		assert( numCols > 0 );
		assert( mNodeAABBs.size() % numCols == 0 );

		return mNodeAABBs.size() / numCols;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetNumNodeElements() const
	{
		assert( !mNodeAABBs.empty() );

		const AABB topLeftAABB = GetAABB( 0 );
		const AABB bottomRightAABB = GetAABB( mNodeAABBs.size() - 1 );

		const Vec3 min = topLeftAABB.Min(), max = bottomRightAABB.Max();
		AABB totalAABB( min, max );

		uint32_t curr = static_cast<uint32_t>( totalAABB.GetExtent().x );
		uint32_t pow = 2;
		// +1 for toplevel AABB node
		uint32_t numAABBs = 1;
		while ( curr > gNodeAABBCutoffPoint )
		{
			numAABBs += 1 << pow;
			curr /= 2;
			pow += 2;
		}

		return numAABBs;
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	AABB FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetAABB( uint32_t index ) const
	{
		const Mat4& aabbTransform = mNodeAABBs.at( index );

		return AABB::gUnitAABB * aabbTransform;
	}
}