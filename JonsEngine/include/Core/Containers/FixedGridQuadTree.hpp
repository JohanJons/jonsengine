#pragma once

#include <vector>
#include <array>

#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

namespace JonsEngine
{
	/*template<typename Item>
	class SingleItemGridNode
	{
	public:
		SingleItemGridNode() = default;

		Item& GetItem() { return mItem; }
		const Item& GetItem() const { return mItem; }

	private:
		Item mItem;
		AABB mAABB;
	};*/

	struct GridQuadNodeAABB
	{
		AABB mAABB;
		//std::array<GridQuadNodeAABB, 4> mNodeQuad;
	};

	template<typename Item>
	class FixedGridQuadTree
	{
	public:
		FixedGridQuadTree( const std::vector<Item>& Items, const std::vector<AABB>& AABBs );

		uint32_t GetGridSize() const { return mNodes.size(); }

		Item& GetNode( uint32_t index ) { return mNodes.at( index ); }
		const Item& GetNode( uint32_t index ) const { return mNodes.at( index ); }

		std::vector<Item>& GetNodes() { return mNodes; }
		const std::vector<Item>& GetNodes() const { return mNodes; }


	private:
		//GridQuadNodeAABB mQuadAABB;
		std::vector<Item> mNodes;
		std::vector<AABB> mNodeAABBs;
		//std::vector<GridQuadNodeAABB> mQuadAABBs;
	};

	typedef FixedGridQuadTree<Mat4> TransformGridQuadTree;

	template<typename Item>
	void FixedGridQuadTree<Item>::Clear()
	{
		mNodes.Clear();
		mNodeAABBs.Clear();
	}
	
	template<typename Item>
	void FixedGridQuadTree<Item>::Insert( Item& Item, AABB aabb )
	{
		assert( mNodes.size() == mNodeAABBs.size() );

		uint32_t offset = 0;
		auto iter = mNodes.begin();
		for ( AABB& aabb : mNodeAABBs )
		{
			

			++offset;
		}
	}
}