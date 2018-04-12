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

	template<typename Item>
	class GridQuadNode
	{
	public:


	private:
		AABB mAABB;
		std::array<Item, 4> NodeQuad;
	};

	template<typename Item>
	class GridQuadTree
	{
	public:
		GridQuadTree() = default;

		uint32_t GetGridSize() const { return mNodes.size(); }

		Item& GetNode( uint32_t index ) { return mNodes.at( index ); }
		const Item& GetNode( uint32_t index ) const { return mNodes.at( index ); }

		std::vector<Item>& GetNodes() { return mNodes; }
		const std::vector<Item>& GetNodes() const { return mNodes; }


	private:
		std::vector<Item> mNodes;
	};

	typedef GridQuadTree<Mat4> TransformGridQuadTree;
}