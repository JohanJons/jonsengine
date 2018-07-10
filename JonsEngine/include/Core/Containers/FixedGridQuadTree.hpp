#pragma once

#include <vector>
#include <array>

#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

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
		uint16_t mBeginIndex = 0, mEndIndex = 0;
	};

	template<typename Item>
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

		std::vector<Item>& GetNodes() { return mNodes; }
		const std::vector<Item>& GetNodes() const { return mNodes; }
		const std::vector<AABB>& GetAABBs() const { return mNodeAABBs; }


	private:
		static constexpr uint32_t gNodeAABBCutoffPoint = 64;

		void CullQuad( std::vector<Item>& Nodes, const GridQuadNodeAABB& QuadAABB, const Mat4& cameraViewProjTransform ) const;

		void BuildAABBTraversal();
		void ProcessQuadNode( GridQuadNodeAABB& quadAABB );
		bool VerifyMemoryLayout() const;
		bool CheckUniformSize() const;
		bool CheckAABBBoundaries( uint32_t numCols ) const;

		uint32_t GetColumn( uint32_t index ) const;
		uint32_t GetRow( uint32_t index ) const;
		uint32_t GetNumColumns() const;
		uint32_t GetNumRows( uint32_t numCols ) const;
		uint32_t GetNumNodeElements() const;

		std::vector<Item> mNodes;
		std::vector<AABB> mNodeAABBs;

		std::vector<GridQuadNodeAABB> mGridTraversal;
	};

	typedef FixedGridQuadTree<Mat4> TransformGridQuadTree;


	template <typename Item>
	FixedGridQuadTree<Item>::FixedGridQuadTree( std::vector<Item>&& Items, std::vector<AABB>&& AABBs )
		: mNodes( Items )
		, mNodeAABBs( AABBs )
	{
		BuildAABBTraversal();
	}

	template <typename Item>
	void FixedGridQuadTree<Item>::CullNodes( std::vector<Item>& nodes, const Mat4& cameraViewProjTransform ) const
	{
		CullQuad( nodes, mGridTraversal.front(), cameraViewProjTransform );
	}

	template <typename Item>
	void FixedGridQuadTree<Item>::CullQuad( std::vector<Item>& Nodes, const GridQuadNodeAABB& QuadAABB, const Mat4& cameraViewProjTransform ) const
	{
		AABBIntersection intersection = Intersection( QuadAABB.mAABB, cameraViewProjTransform );

	}

	template <typename Item>
	void FixedGridQuadTree<Item>::BuildAABBTraversal()
	{
		assert( mNodes.size() == mNodeAABBs.size() );
		assert( VerifyMemoryLayout() );

		uint32_t numCols = GetNumColumns();
		uint32_t numRows = GetNumRows( numCols );

		// only square quads for now
		assert( numCols == numRows );

		mGridTraversal.reserve( GetNumNodeElements() );

		AABB& topLeftAABB = mNodeAABBs.front();
		AABB& bottomRightAABB = mNodeAABBs.back();
		mGridTraversal.emplace_back( topLeftAABB.Min(), bottomRightAABB.Max(), 0, ( numCols * numRows ) - 1 );
		GridQuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB );
	}

	template <typename Item>
	void FixedGridQuadTree<Item>::ProcessQuadNode( GridQuadNodeAABB& quadAABB )
	{
		if ( quadAABB.mAABB.GetExtent().x <= static_cast<float>( gNodeAABBCutoffPoint ) )
			return;

		const Vec3 splitExtent( quadAABB.mAABB.GetExtent().x, 0, 0 );
		const Vec3 tlAABBMin = quadAABB.mAABB.Min(), brAABBMax = quadAABB.mAABB.Max();
		const Vec3 center = quadAABB.mAABB.GetCenter();

		const uint16_t parentBegin = quadAABB.mBeginIndex, parentEnd = quadAABB.mEndIndex;

		uint32_t beginColumn = GetColumn( parentBegin ), beginRow = GetRow( parentBegin );
		uint32_t endColumn = GetColumn( parentEnd ), endRow = GetRow( parentEnd );

		mGridTraversal.emplace_back( tlAABBMin, center, parentBegin, parentEnd / 2 );
		quadAABB.mChildBegin = &mGridTraversal.back();
		mGridTraversal.emplace_back( tlAABBMin + splitExtent, center + splitExtent, 0, 0 );
		mGridTraversal.emplace_back( center - splitExtent, brAABBMax - splitExtent, 0, 0 );
		mGridTraversal.emplace_back( center, brAABBMax, 0, 0 );

		for ( uint32_t offset = GridQuadNodeAABB::TOP_LEFT; offset < GridQuadNodeAABB::NUM_CHILDREN; ++offset )
			ProcessQuadNode( *( quadAABB.mChildBegin + offset ) );
	}

	template <typename Item>
	bool FixedGridQuadTree<Item>::VerifyMemoryLayout() const
	{
		if ( mNodeAABBs.size() <= 0 )
		{
			return true;
		}
		
		// some unnecessary re-traversal inflicted
		CheckUniformSize();
		uint32_t numCols = GetNumColumns();
		assert( numCols > 0 );
		if ( mNodeAABBs.size() % numCols != 0 )
			return false;

		uint32_t numRows = GetNumRows(numCols );
		if ( numCols * numRows != mNodeAABBs.size() )
			return false;

		return CheckAABBBoundaries( numCols );
	}

	template <typename Item>
	bool FixedGridQuadTree<Item>::CheckUniformSize() const
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

	template <typename Item>
	bool FixedGridQuadTree<Item>::CheckAABBBoundaries( uint32_t numCols ) const
	{
		assert( numCols >= 1 );

		for ( uint32_t index = 0, numNodes = mNodes.size() - numCols; index < numNodes; ++index )
		{
			const AABB* currAABB = &mNodeAABBs[ index ];
			const AABB* nextAABB = &mNodeAABBs[ index + 1 ];
			const AABB* bottomAABB = &mNodeAABBs[ index + numCols ];

			Vec3 currMin = currAABB->Min(), currMax = currAABB->Max();
			Vec3 nextMin = nextAABB->Min();
			Vec3 bottomMin = bottomAABB->Min();

			bool bIsNotAtEndColumn = ( index + 1 ) % numCols != 0;
			if ( bIsNotAtEndColumn && currMax.x != nextMin.x )
				return false;

			if ( currMax.z != bottomMin.z )
				return false;
		}

		return true;
	}

	template <typename Item>
	uint32_t FixedGridQuadTree<Item>::GetColumn( uint32_t index ) const
	{
		return index % GetNumColumns();
	}

	template <typename Item>
	uint32_t FixedGridQuadTree<Item>::GetRow( uint32_t index ) const
	{
		uint32_t columns = GetNumColumns();

		return index / GetNumRows( columns );
	}

	template <typename Item>
	uint32_t FixedGridQuadTree<Item>::GetNumColumns() const
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

	template <typename Item>
	uint32_t FixedGridQuadTree<Item>::GetNumRows( uint32_t numCols ) const
	{
		assert( numCols > 0 );
		assert( mNodeAABBs.size() % numCols == 0 );

		return mNodeAABBs.size() / numCols;
	}

	template <typename Item>
	uint32_t FixedGridQuadTree<Item>::GetNumNodeElements() const
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