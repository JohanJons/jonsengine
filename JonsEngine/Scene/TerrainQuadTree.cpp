#include "Scene/TerrainQuadTree.h"

#include "Core/Math/Intersection.h"
#include "Core/Math/MathUtils.h"

#include <set>

namespace
{
	using namespace JonsEngine;

	void GetTransformExtentsXZ( const Mat4& transform, Vec2& min, Vec2& max )
	{
		const Vec4& translation = transform[ 3 ];
		float xExtent = transform[ 0 ].x / 2;
		float zExtent = transform[ 2 ].z / 2;

		min = Vec2( translation.x - xExtent, translation.z - zExtent );
		max = Vec2( translation.x + xExtent, translation.z + zExtent );
	}
}

namespace JonsEngine
{
	TerrainQuadTree::TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform ) :
		mPatchMinSize( patchMinSize ),
		mHeightmapScale( heightmapScale )
	{
		float fWidth = static_cast<float>( width );
		float fHeight = static_cast<float>( height );

		assert( patchMinSize && fWidth >= 0.0f && fHeight >= 0.0f && !heightmapData.empty() );

		uint32_t numNodes = ExpectedNumNodes( fWidth, mPatchMinSize );
		mGridTraversal.reserve( numNodes );
		
		float yTranslation = worldTransform[ 3 ].y;
		uint32_t rootLODLevel = 0;
		CreateGridNode( nullptr, fWidth / 2, fHeight / 2, fWidth, fHeight, rootLODLevel, yTranslation );
		QuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB, heightmapData, width, rootLODLevel, yTranslation );

		// translate negative width&height / 2 to center it at origo
		Mat4 finalTransform = glm::translate( -Vec3( fWidth / 2, 0, fHeight / 2) );
		finalTransform = worldTransform * finalTransform;

		for ( QuadNodeAABB& node : mGridTraversal )
			node.mFrustumAABB = node.mFrustumAABB * finalTransform;

		assert( numNodes == GetNumNodes() );
	}

	void TerrainQuadTree::CullNodes( std::vector<RenderableTerrainQuad>& renderableQuads, std::vector<float>& LODRanges, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const
	{
		renderableQuads.clear();
		LODRanges.clear();
		//tessEdgeMult.clear();

		CalculateLODRanges( LODRanges, zNear, zFar );
		CullQuad( renderableQuads, mGridTraversal.front(), );

		//PerCullData cullData( GetNumNodes() );
		//CalculateHighestLODNodes( cullData, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );
		//CalculateNodeTransforms( cullData, nodeTransforms, tessEdgeMult, LODRanges );
		//CullQuad( nodeTransforms, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );

		//CalculateTessellationFactors( nodeTransforms, tessEdgeMult );

		//assert( nodeTransforms.size() == tessEdgeMult.size() );
	}

	uint32_t TerrainQuadTree::GetNumLODRanges() const
	{
		return static_cast<uint32_t>( std::log( 3 * GetNumNodes() + 1 ) / std::log( 4 ) );
	}

	void TerrainQuadTree::GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const
	{
		const QuadNodeAABB& toplevelNode = mGridTraversal.front();
		Vec3 minAABB = toplevelNode.mFrustumAABB.Min(), maxAABB = toplevelNode.mFrustumAABB.Max();

		worldMin = Vec2( minAABB.x, minAABB.z );
		worldMax = Vec2( maxAABB.x, maxAABB.z );
	}

	uint32_t TerrainQuadTree::ExpectedNumNodes( float width, uint32_t patchMinSize ) const
	{
		assert( width && patchMinSize );

		uint32_t curr = static_cast<uint32_t>( width );
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

	void TerrainQuadTree::CreateGridNode( QuadNodeAABB* parent, float centerX, float centerZ, float width, float height, uint32_t LODlevel, float yTranslation )
	{
		float extentX = width / 2;
		float extentZ = height / 2;

		float minX = centerX - extentX, maxX = centerX + extentX;
		float minZ = centerZ - extentZ, maxZ = centerZ + extentZ;

		Vec3 frustumMin( minX, 0, minZ );
		Vec3 frustumMax( maxX, 0, maxZ );

		mGridTraversal.emplace_back( frustumMin, frustumMax, parent, LODlevel );
	}

	void TerrainQuadTree::ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth, uint32_t LODlevel, float yTranslation )
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();
		Vec3 min = quadAABB.mFrustumAABB.Min();
		Vec3 max = quadAABB.mFrustumAABB.Max();

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
			quadAABB.mFrustumAABB = AABB( min, max );

			return;
		}

		float centerX = center.x, centerZ = center.z;
		float childWidth = extent.x;
		float childHeight = extent.z;
		float halfChildWidth = childWidth / 2;
		float halfChildHeight = childHeight / 2;
		uint32_t childNodeLODLevel = LODlevel + 1;

		// BL - BR - TR - TL
		// TODO: validate
		CreateGridNode( &quadAABB, centerX - halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		quadAABB.mChildBegin = &mGridTraversal.back();
		CreateGridNode( &quadAABB, centerX + halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		CreateGridNode( &quadAABB, centerX + halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		CreateGridNode( &quadAABB, centerX - halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );

		// process children and collect min/max Y for AABB
		for ( uint32_t childIndex = QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT; childIndex < QuadChildEnum::QUAD_CHILD_COUNT; ++childIndex )
		{
			QuadNodeAABB& childNode = *( quadAABB.mChildBegin + childIndex );
			ProcessQuadNode( childNode, heightmapData, heightmapWidth, childNodeLODLevel, yTranslation );

			minY = std::min( minY, childNode.mFrustumAABB.Min().y );
			maxY = std::max( maxY, childNode.mFrustumAABB.Max().y );
		}

		min.y = minY;
		max.y = maxY;
		quadAABB.mFrustumAABB = AABB( min, max );
	}

	void TerrainQuadTree::CalculateLODRanges( std::vector<float>& LODs, float zNear, float zFar ) const
	{
		uint32_t numLODRanges = GetNumLODRanges();
		LODs.resize( numLODRanges );

		const float LODDistanceRatio = 2.0f;

		float total = 0;
		float currentDetailBalance = 1.0f;
		for ( uint32_t i = 0; i < numLODRanges; ++i )
		{
			total += currentDetailBalance;
			currentDetailBalance *= LODDistanceRatio;
		}

		float sect = ( zFar - zNear ) / total;

		float prevPos = zNear;
		currentDetailBalance = 1.0f;
		for ( uint32_t i = 0; i < numLODRanges; ++i )
		{
			LODs[ numLODRanges - i - 1 ] = prevPos + sect * currentDetailBalance;
			prevPos = LODs[ numLODRanges - i - 1 ];
			currentDetailBalance *= LODDistanceRatio;
		}
	}

	void TerrainQuadTree::AddNode( std::vector<RenderableTerrainQuad>& renderableQuads, const QuadNodeAABB& quadAABB ) const
	{
		AddNode( renderableQuads, quadAABB, true, true, true, true );
	}

	void TerrainQuadTree::AddNode( std::vector<RenderableTerrainQuad>& renderableQuads, const QuadNodeAABB& quadAABB, bool addBL, bool addBR, bool addTR, bool addTL ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();

		Mat4 transform = glm::translate( Vec3( center.x, 0.0f, center.z ) );
		transform = glm::scale( transform, extent * 2.0f );

		std::bitset<QuadChildEnum::QUAD_CHILD_COUNT> bitset;
		bitset.set( QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT, addBL );
		bitset.set( QuadChildEnum::QUAD_CHILD_BOTTOM_RIGHT, addBR );
		bitset.set( QuadChildEnum::QUAD_CHILD_TOP_RIGHT, addTR );
		bitset.set( QuadChildEnum::QUAD_CHILD_TOP_LEFT, addTL );

		renderableQuads.emplace_back( transform, quadAABB.mLODIndex, bitset );
	}

	QuadNodeCullStatus TerrainQuadTree::CullQuad( std::vector<RenderableTerrainQuad>& renderableQuads, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const
	{
		// if parent is fully in frustum, so are we
		AABBIntersection frustumIntersection = parentFullyInFrustum ? AABBIntersection::Inside : Intersection( quadAABB.mFrustumAABB, cameraViewProjTransform );
		if ( frustumIntersection == AABBIntersection::Outside )
			return QuadNodeCullStatus::OutOfFrustum;

		float distanceLimit = LODRanges.at( quadAABB.mLODIndex );
		AABBIntersection intersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, distanceLimit );
		bool isWithinDistance = intersection == AABBIntersection::Partial || intersection == AABBIntersection::Inside;
		if ( !isWithinDistance )
			return QuadNodeCullStatus::OutOfRange;

		// last LOD
		uint32_t nextLODIndex = quadAABB.mLODIndex + 1;
		bool isValidLODIndex = nextLODIndex < LODRanges.size();
		if ( !isValidLODIndex )
		{
			AddNode( renderableQuads, quadAABB );
			return QuadNodeCullStatus::Added;
		}

		float nextDistanceLimit = LODRanges.at( nextLODIndex );
		AABBIntersection nextIntersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, nextDistanceLimit );
		bool nextIsWithinRange = nextIntersection == AABBIntersection::Partial || nextIntersection == AABBIntersection::Inside;
		if ( !nextIsWithinRange )
		{
			AddNode( renderableQuads, quadAABB );
			return QuadNodeCullStatus::Added;
		}

		QuadNodeCullStatus Status = QuadNodeCullStatus::OutOfFrustum;
		bool completelyInFrustum = frustumIntersection == AABBIntersection::Inside;
		std::array<QuadNodeCullStatus, QuadChildEnum::QUAD_CHILD_COUNT> childrenAdded;
		for ( uint32_t childIndex = QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT; childIndex < QuadChildEnum::QUAD_CHILD_COUNT; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			childrenAdded[ childIndex ] = CullQuad( renderableQuads, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges, completelyInFrustum );
		}

		// remove if childnodes already selected, or out of frustum
		bool removeBL = childrenAdded[ 0 ] == QuadNodeCullStatus::Added || childrenAdded[ 0 ] == QuadNodeCullStatus::OutOfRange;
		bool removeBR = childrenAdded[ 1 ] != QuadNodeCullStatus::Added || childrenAdded[ 1 ] == QuadNodeCullStatus::OutOfRange;
		bool removeTR = childrenAdded[ 2 ] != QuadNodeCullStatus::Added || childrenAdded[ 2 ] == QuadNodeCullStatus::OutOfRange;
		bool removeTL = childrenAdded[ 3 ] != QuadNodeCullStatus::Added || childrenAdded[ 3 ] == QuadNodeCullStatus::OutOfRange;
		bool doAddSelf = !removeBL || !removeBR || !removeTR || !removeTL;

		if ( doAddSelf )
		{
			AddNode( renderableQuads, quadAABB, !removeBL, !removeBR, !removeTR, !removeTL );

			Status = QuadNodeCullStatus::Added;
		}
		else
			Status = QuadNodeCullStatus::OutOfFrustum;

		return Status;
	}
}
