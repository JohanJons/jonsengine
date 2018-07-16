#pragma once

#include <vector>
#include <array>

#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"
#include "include/RenderQueue/RenderQueueTypes.h"

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

		GridQuadNodeAABB( const Vec3& minAABB, const Vec3& maxAABB, uint16_t beginIndex, uint16_t endIndex )
			: mAABB( minAABB, maxAABB )
			, mBeginIndex( beginIndex )
			, mEndIndex( endIndex )
		{ }

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
		FixedGridQuadTree( std::vector<Item>&& Items, std::vector<AABB>&& AABBs );

		uint32_t GetGridSize() const { return mNodes.size(); }

		Item& GetNode( uint32_t index ) { return mNodes.at( index ); }
		const Item& GetNode( uint32_t index ) const { return mNodes.at( index ); }

		// AABBs are in worldspace already
		void CullNodes( std::vector<Item>& nodes, const Mat4& cameraViewProjTransform ) const;

	private:
		void CullQuad( std::vector<Item>& nodes, const GridQuadNodeAABB& quadAABB, const Mat4& cameraViewProjTransform ) const;
		void AddAllItems( std::vector<Item>& nodes, const GridQuadNodeAABB& quadAABB ) const;

		void BuildAABBTraversal();
		void ProcessQuadNode( GridQuadNodeAABB& quadAABB );
		bool VerifyMemoryLayout() const;
		bool CheckUniformSize() const;
		bool CheckAABBBoundaries() const;

		uint32_t GetIndex( uint32_t rowIndex, uint32_t columnIndex ) const;
		uint32_t GetColumn( uint32_t index ) const;
		uint32_t GetRow( uint32_t index ) const;
		uint32_t GetNumColumns() const;
		uint32_t GetNumRows( uint32_t numCols ) const;
		uint32_t GetNumNodeElements() const;

		std::vector<Item> mNodes;
		std::vector<AABB> mNodeAABBs;

		uint32_t mNumColumns;
		uint32_t mNumRows;

		std::vector<GridQuadNodeAABB> mGridTraversal;
	};


	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::FixedGridQuadTree( std::vector<Item>&& Items, std::vector<AABB>&& AABBs )
		: mNodes( Items )
		, mNodeAABBs( AABBs )
		, mNumColumns( GetNumColumns() )
		, mNumRows( GetNumRows( mNumColumns ) )
	{
		BuildAABBTraversal();
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CullNodes( std::vector<Item>& nodes, const Mat4& cameraViewProjTransform ) const
	{
		assert( !mGridTraversal.empty() );
		CullQuad( nodes, mGridTraversal.front(), cameraViewProjTransform );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::CullQuad( std::vector<Item>& nodes, const GridQuadNodeAABB& quadAABB, const Mat4& cameraViewProjTransform ) const
	{
		AABBIntersection intersection = Intersection( quadAABB.mAABB, cameraViewProjTransform );
		switch ( intersection )
		{
			case AABBIntersection::Partial:
			{
				if ( !quadAABB.mChildBegin )
					AddAllItems( nodes, quadAABB );

				for ( uint32_t childIndex = GridQuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < GridQuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
					CullQuad( nodes, *( quadAABB.mChildBegin + childIndex ), cameraViewProjTransform );

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
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::AddAllItems( std::vector<Item>& nodes, const GridQuadNodeAABB& quadAABB ) const
	{
		uint32_t begin = quadAABB.mBeginIndex, end = quadAABB.mEndIndex;
		uint32_t beginRow = GetRow( begin ), endRow = GetRow( end );
		uint32_t beginCol = GetColumn( begin ), endCol = GetColumn( end );
		assert( beginRow >= 0 && endRow >= beginRow && endRow < mNumRows );
		assert( beginCol >= 0 && endCol >= beginCol && endCol < mNumColumns );

		// endRow/endColumn are inclusive indices
		for ( uint32_t i = beginRow; i <= endRow; ++i )
		{
			uint32_t rowBeginIndex = GetIndex( i, beginCol ), rowEndIndex = GetIndex( i, endCol );
			nodes.insert( nodes.end(), mNodes.begin() + rowBeginIndex, mNodes.begin() + rowEndIndex + 1 );
		}
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::BuildAABBTraversal()
	{
		assert( mNodes.size() == mNodeAABBs.size() );
		assert( VerifyMemoryLayout() );

		// only square quads for now
		assert( mNumColumns == mNumRows );

		mGridTraversal.reserve( GetNumNodeElements() );

		AABB& topLeftAABB = mNodeAABBs.front();
		AABB& bottomRightAABB = mNodeAABBs.back();
		mGridTraversal.emplace_back( topLeftAABB.Min(), bottomRightAABB.Max(), 0, ( mNumColumns * mNumRows ) - 1 );
		GridQuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	void FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::ProcessQuadNode( GridQuadNodeAABB& quadAABB )
	{
		if ( quadAABB.mAABB.GetExtent().x <= static_cast<float>( gNodeAABBCutoffPoint ) )
			return;

		const Vec3 splitExtent( quadAABB.mAABB.GetExtent().x, 0, 0 );
		const Vec3 tlAABBMin = quadAABB.mAABB.Min(), brAABBMax = quadAABB.mAABB.Max();
		const Vec3 center = quadAABB.mAABB.GetCenter();

		const uint16_t parentBegin = quadAABB.mBeginIndex, parentEnd = quadAABB.mEndIndex;

		uint32_t beginColumn = GetColumn( parentBegin ), beginRow = GetRow( parentBegin );
		uint32_t endColumn = GetColumn( parentEnd ), endRow = GetRow( parentEnd );
		assert( endColumn > beginColumn && endRow > beginRow );

		uint32_t numCols = endColumn - beginColumn, numRows = endRow - beginRow;
		uint32_t indexTL = GetIndex( beginRow, beginColumn ), indexTM = GetIndex( beginRow, beginColumn + numCols / 2 );
		uint32_t indexML = GetIndex( beginRow + numRows / 2, beginColumn ), indexMM = GetIndex( beginRow + numRows / 2, beginColumn + numCols / 2 ), indexMR = GetIndex( beginRow + numRows / 2, endColumn );
		uint32_t indexLM = GetIndex( endRow, beginColumn + numCols / 2 ), indexLR = GetIndex( endRow, endColumn );

		mGridTraversal.emplace_back( tlAABBMin, center, indexTL, indexMM );
		quadAABB.mChildBegin = &mGridTraversal.back();
		mGridTraversal.emplace_back( tlAABBMin + splitExtent, center + splitExtent, indexTM, indexMR );
		mGridTraversal.emplace_back( center - splitExtent, brAABBMax - splitExtent, indexML, indexLM );
		mGridTraversal.emplace_back( center, brAABBMax, indexMM, indexLR );

		for ( uint32_t offset = GridQuadNodeAABB::TOP_LEFT; offset < GridQuadNodeAABB::NUM_CHILDREN; ++offset )
			ProcessQuadNode( *( quadAABB.mChildBegin + offset ) );
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

		const AABB* prevAABB = &mNodeAABBs[ 0 ];
		for ( uint32_t index = 1, numNodes = mNodes.size(); index < numNodes; ++index )
		{
			const AABB* currAABB = &mNodeAABBs[ index ];

			if ( prevAABB->GetExtent() != currAABB->GetExtent() )
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
			const AABB* currAABB = &mNodeAABBs[ index ];
			const AABB* nextAABB = &mNodeAABBs[ index + 1 ];
			const AABB* bottomAABB = &mNodeAABBs[ index + mNumColumns ];

			Vec3 currMin = currAABB->Min(), currMax = currAABB->Max();
			Vec3 nextMin = nextAABB->Min();
			Vec3 bottomMin = bottomAABB->Min();

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
		assert( index >= 0 && index < mGridTraversal.size() );

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
		return index / GetNumRows( mNumColumns );
	}

	template <typename Item, uint32_t gNodeAABBCutoffPoint>
	uint32_t FixedGridQuadTree<Item, gNodeAABBCutoffPoint>::GetNumColumns() const
	{
		assert( !mNodeAABBs.empty() );

		uint32_t numCols = 1;
		const AABB* prevAABB = &mNodeAABBs[ 0 ];
		for ( uint32_t index = 1, numNodes = mNodes.size(); index < numNodes; ++index )
		{
			const AABB* currAABB = &mNodeAABBs[ index ];

			Vec3 currCenter = currAABB->GetCenter(), currExtent = currAABB->GetExtent();
			Vec3 prevCenter = prevAABB->GetCenter(), prevExtent = prevAABB->GetExtent();

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

		const AABB& topLeftAABB = mNodeAABBs.front();
		const AABB& bottomRightAABB = mNodeAABBs.back();
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
}