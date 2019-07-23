#include "Scene/TerrainQuadTree.h"

#include "Core/Math/Intersection.h"
#include "Core/Math/MathUtils.h"

namespace JonsEngine
{
	TerrainQuadTree::TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform ) :
		mPatchMinSize( patchMinSize ),
		mHeightmapScale( heightmapScale )
	{
		assert( patchMinSize && width && height && !heightmapData.empty() );

		mGridTraversal.reserve( ExpectedNumNodes( width, mPatchMinSize ) );

		AddGridNode( width / 2, height / 2, width, height );
		QuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB, heightmapData, width );

		for ( QuadNodeAABB& node : mGridTraversal )
			node.mAABB = node.mAABB * worldTransform;

		assert( ExpectedNumNodes( width, mPatchMinSize ) == GetNumNodes() );
	}

	void TerrainQuadTree::CullNodes( std::vector<Mat4>& nodeTransforms, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const
	{
		std::vector<float> LODRanges;
		CalculateLODRanges( LODRanges, zNear, zFar );

		CullQuad( nodeTransforms, mGridTraversal.front(), cameraViewProjTransform );
	}

	uint32_t TerrainQuadTree::GetNumLODRanges() const
	{
		return static_cast<uint32_t>( ( std::log( 3 * GetNumNodes() + 1 ) / std::log( 4 ) ) - 1 );
	}

	void TerrainQuadTree::GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const
	{
		const QuadNodeAABB& toplevelNode = mGridTraversal.front();
		Vec3 minAABB = toplevelNode.mAABB.Min(), maxAABB = toplevelNode.mAABB.Max();

		worldMin = Vec2( minAABB.x, minAABB.z );
		worldMax = Vec2( maxAABB.x, maxAABB.z );
	}

	void TerrainQuadTree::CullQuad( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Mat4& cameraViewProjTransform ) const
	{
		/*AABBIntersection intersection = Intersection( quadAABB.mAABB, cameraViewProjTransform );
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
		}*/
	}

	uint32_t TerrainQuadTree::ExpectedNumNodes( uint32_t width, uint32_t patchMinSize ) const
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

	void TerrainQuadTree::AddGridNode( uint32_t centerX, uint32_t centerZ, uint32_t width, uint32_t height )
	{
		uint32_t extentX = width / 2;
		uint32_t extentZ = height / 2;

		Vec3 min( centerX - extentX, 0, centerZ - extentZ );
		Vec3 max( centerX + extentX, 0, centerZ + extentZ );

		mGridTraversal.emplace_back( min, max );
	}

	void TerrainQuadTree::ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth )
	{
		Vec3 center = quadAABB.mAABB.GetCenter();
		Vec3 extent = quadAABB.mAABB.GetExtent();
		Vec3 min = quadAABB.mAABB.Min();
		Vec3 max = quadAABB.mAABB.Max();

		constexpr float inf = std::numeric_limits<float>::infinity();
		float minY = inf, maxY = -inf;

		uint32_t quadWidth = static_cast<uint32_t>( extent.x * 2 ), quadHeight = static_cast<uint32_t>( extent.z * 2 );
		if ( quadWidth <= mPatchMinSize && quadHeight <= mPatchMinSize )
		{
			//leaf node; parse in heightmap data to complete AABB, which will seep upwards
			for ( uint32_t col = static_cast<uint32_t>( min.x ); col < max.x; ++col )
			{
				for ( uint32_t row = static_cast<uint32_t>( min.z ); row < max.z; ++row )
				{
					uint32_t index = col + ( row * heightmapWidth );
					float val = Normalize( heightmapData[ index ] ) * mHeightmapScale;

					minY = std::min( minY, val );
					maxY = std::max( maxY, val );
				}
			}

			min.y = minY;
			max.y = maxY;
			quadAABB.mAABB = AABB( min, max );

			return;
		}

		uint32_t centerX = static_cast<uint32_t>( center.x ), centerZ = static_cast<uint32_t>( center.z );
		uint32_t childWidth = static_cast<uint32_t>( extent.x );
		uint32_t childHeight = static_cast<uint32_t>( extent.z );
		uint32_t halfChildWidth = childWidth / 2;
		uint32_t halfChildHeight = childHeight / 2;

		// TL - TR - BL - BR
		AddGridNode( centerX - halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight );
		quadAABB.mChildBegin = &mGridTraversal.back();
		AddGridNode( centerX + halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight );
		AddGridNode( centerX + halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight );
		AddGridNode( centerX - halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight );

		// process children and collect min/max Y for AABB
		for ( uint32_t offset = QuadNodeAABB::TOP_LEFT; offset < QuadNodeAABB::NUM_CHILDREN; ++offset )
		{
			QuadNodeAABB& childNode = *( quadAABB.mChildBegin + offset );
			ProcessQuadNode( childNode, heightmapData, heightmapWidth );

			minY = std::min( minY, childNode.mAABB.Min().y );
			maxY = std::max( maxY, childNode.mAABB.Max().y );
		}

		min.y = minY;
		max.y = maxY;
		quadAABB.mAABB = AABB( min, max );
	}

	void TerrainQuadTree::CalculateLODRanges( std::vector<float>& LODs, float zNear, float zFar ) const
	{
		uint32_t numLODRanges = GetNumLODRanges();
		LODs.reserve( numLODRanges );

		//LODLevelDistanceRatio         = 2.0

		float sect = ( zFar - zNear ) / total;
	}
}

