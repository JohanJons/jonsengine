#include "Scene/TerrainQuadTree.h"

#include "Core/Math/Intersection.h"
#include "Core/Math/MathUtils.h"

#include <set>

namespace
{
	using namespace JonsEngine;

	constexpr float gMorphStartRatio = 0.667f;

	void GetTransformExtentsXZ( const Mat4& transform, Vec2& min, Vec2& max )
	{
		const Vec4& translation = transform[ 3 ];
		float xExtent = transform[ 0 ].x / 2;
		float zExtent = transform[ 2 ].z / 2;

		min = Vec2( translation.x - xExtent, translation.z - zExtent );
		max = Vec2( translation.x + xExtent, translation.z + zExtent );
	}

	bool IsPatchTooLarge( const QuadNodeAABB& quadAABB, uint32_t maxPatchSize )
	{
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();
		bool isTooLargePatch = ( static_cast<uint32_t>( extent.x * 2.0f ) > maxPatchSize || static_cast<uint32_t>( extent.z * 2.0f > maxPatchSize ) );

		return isTooLargePatch;
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

	void TerrainQuadTree::CullNodes( std::vector<Mat4>& renderableTransforms, std::vector<float>& LODRanges, std::vector<Vec2>& morphConstants, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform,
		float zNear, float zFar, uint32_t maxPatchSize ) const
	{
		renderableTransforms.clear();
		LODRanges.clear();
		morphConstants.clear();

		const QuadNodeAABB& rootNode = mGridTraversal.front();
		bool isTooLargePatch = IsPatchTooLarge( rootNode, maxPatchSize );

		CalculateLODRanges( LODRanges, morphConstants, zNear, zFar, maxPatchSize );
		CullQuad( renderableTransforms, rootNode, cameraWorldPos, cameraViewProjTransform, LODRanges, false, isTooLargePatch, maxPatchSize );

#if _DEBUG
		assert( ValidateCulledNodes( renderableTransforms ) ); 
#endif

		//PerCullData cullData( GetNumNodes() );
		//CalculateHighestLODNodes( cullData, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );
		//CalculateNodeTransforms( cullData, nodeTransforms, tessEdgeMult, LODRanges );
		//CullQuad( nodeTransforms, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );

		//CalculateTessellationFactors( nodeTransforms, tessEdgeMult );

		//assert( nodeTransforms.size() == tessEdgeMult.size() );
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

	bool TerrainQuadTree::ValidateCulledNodes( std::vector<Mat4>& renderableTransforms ) const
	{
		for ( uint32_t index = 0; index < renderableTransforms.size(); ++index )
		{
			const Mat4& firstMat = renderableTransforms.at( index );

			Vec2 min, max;
			GetTransformExtentsXZ( firstMat, min, max );

			for ( uint32_t otherIndex = index + 1; otherIndex < renderableTransforms.size(); ++otherIndex )
			{
				const Mat4& secondMat = renderableTransforms.at( otherIndex );

				Vec2 otherMin, otherMax;
				GetTransformExtentsXZ( secondMat, otherMin, otherMax );

				if ( ( otherMin.x <= min.x && otherMax.x > min.x ) || // to the left
					( otherMax.x >= max.x && otherMin.x < max.x ) ) // to the right
				{
					if ( ( otherMin.y <= min.y && otherMax.y > min.y ) ||
						( otherMax.y >= max.y && otherMin.y < max.y ) )
						return false;
				}
			}
		}

		return true;
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

	void TerrainQuadTree::CalculateLODRanges( std::vector<float>& LODs, std::vector<Vec2>& morphConstants, float zNear, float zFar, uint32_t maxPatchSize ) const
	{
		uint32_t numLODRanges = static_cast<uint32_t>( sqrt( maxPatchSize / mPatchMinSize ) ) + 1;

		LODs.resize( numLODRanges );
		morphConstants.resize( numLODRanges );

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
			uint32_t index = numLODRanges - i - 1;

			LODs[ index ] = prevPos + sect * currentDetailBalance;
			prevPos = LODs[ index ];
			currentDetailBalance *= LODDistanceRatio;
		}

		prevPos = zNear;
		for ( uint32_t i = 0; i < numLODRanges; ++i )
		{
			uint32_t index = numLODRanges - i - 1;

			float morphEnd = LODs[ index ];
			float morphStart = prevPos + ( morphEnd - prevPos ) * gMorphStartRatio;

			prevPos = morphEnd;

			const float errorFudge = 0.01f;
			morphEnd = glm::lerp( morphEnd, morphStart, errorFudge );
			
			morphConstants[ index ].x = morphEnd / ( morphEnd - morphStart );
			morphConstants[ index ].y = 1.0f / ( morphEnd - morphStart );
		}
	}

	void TerrainQuadTree::AddNode( std::vector<Mat4>& renderableTransforms, const QuadNodeAABB& quadAABB, uint32_t maxPatchSize, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();

		assert( extent.x * 2.0f <= maxPatchSize && extent.z * 2.0f <= maxPatchSize );

		Mat4 transform = glm::translate( Vec3( center.x, 0.0f, center.z ) );
		transform = glm::scale( transform, extent * 2.0f );

		renderableTransforms.emplace_back( transform );
	}

	void TerrainQuadTree::AddNode( std::vector<Mat4>& renderableTransforms, const QuadNodeAABB& quadAABB, uint32_t maxPatchSize, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum, bool addBL, bool addBR, bool addTR, bool addTL ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();
		Vec3 halfExtent = extent / 2.0f;

		if ( addBL )
			AddNode( renderableTransforms, *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT ), maxPatchSize, cameraViewProjTransform, parentFullyInFrustum );

		if ( addBR )
			AddNode( renderableTransforms, *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_BOTTOM_RIGHT ), maxPatchSize, cameraViewProjTransform, parentFullyInFrustum );

		if ( addTR )
			AddNode( renderableTransforms, *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_TOP_RIGHT ), maxPatchSize, cameraViewProjTransform, parentFullyInFrustum );

		if ( addTL )
			AddNode( renderableTransforms, *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_TOP_LEFT ), maxPatchSize, cameraViewProjTransform, parentFullyInFrustum );
	}

	QuadNodeCullStatus TerrainQuadTree::CullQuad( std::vector<Mat4>& renderableTransforms, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform,
		const std::vector<float>& LODRanges, bool parentFullyInFrustum, bool parentTooLargePatch, uint32_t maxPatchSize ) const
	{
		// if parent is fully in frustum, so are we
		AABBIntersection frustumIntersection = parentFullyInFrustum ? AABBIntersection::Inside : Intersection( quadAABB.mFrustumAABB, cameraViewProjTransform );
		if ( frustumIntersection == AABBIntersection::Outside )
			return QuadNodeCullStatus::OutOfFrustum;

		float distanceLimit = LODRanges.at( quadAABB.mLODIndex );
		AABBIntersection intersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, distanceLimit );
		bool isWithinDistance = intersection == AABBIntersection::Partial || intersection == AABBIntersection::Inside;
		if ( !parentTooLargePatch && !isWithinDistance )
			return QuadNodeCullStatus::OutOfRange;

		bool completelyInFrustum = frustumIntersection == AABBIntersection::Inside;

		// last LOD
		uint32_t nextLODIndex = quadAABB.mLODIndex + 1;
		bool isValidLODIndex = nextLODIndex < LODRanges.size();
		if ( !isValidLODIndex )
		{
			AddNode( renderableTransforms, quadAABB, maxPatchSize, cameraViewProjTransform, completelyInFrustum );
			return QuadNodeCullStatus::Added;
		}

		bool isTooLargePatch = IsPatchTooLarge( quadAABB, maxPatchSize );

		float nextDistanceLimit = LODRanges.at( nextLODIndex );
		AABBIntersection nextIntersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, nextDistanceLimit );
		bool nextIsWithinRange = nextIntersection == AABBIntersection::Partial || nextIntersection == AABBIntersection::Inside;
		if ( !isTooLargePatch && !nextIsWithinRange )
		{
			AddNode( renderableTransforms, quadAABB, maxPatchSize, cameraViewProjTransform, completelyInFrustum );
			return QuadNodeCullStatus::Added;
		}

		std::array<QuadNodeCullStatus, QuadChildEnum::QUAD_CHILD_COUNT> childrenAdded{ QuadNodeCullStatus::Undefined, QuadNodeCullStatus::Undefined, QuadNodeCullStatus::Undefined, QuadNodeCullStatus::Undefined };
		for ( uint32_t childIndex = QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT; childIndex < QuadChildEnum::QUAD_CHILD_COUNT; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			childrenAdded[ childIndex ] = CullQuad( renderableTransforms, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges, completelyInFrustum, isTooLargePatch, maxPatchSize );
		}

		// remove if childnodes already selected, or out of frustum
		bool removeBL = childrenAdded[ 0 ] == QuadNodeCullStatus::Added || childrenAdded[ 0 ] == QuadNodeCullStatus::OutOfFrustum;
		bool removeBR = childrenAdded[ 1 ] == QuadNodeCullStatus::Added || childrenAdded[ 1 ] == QuadNodeCullStatus::OutOfFrustum;
		bool removeTR = childrenAdded[ 2 ] == QuadNodeCullStatus::Added || childrenAdded[ 2 ] == QuadNodeCullStatus::OutOfFrustum;
		bool removeTL = childrenAdded[ 3 ] == QuadNodeCullStatus::Added || childrenAdded[ 3 ] == QuadNodeCullStatus::OutOfFrustum;
		bool doAddSelf = !( removeBL && removeBR && removeTR && removeTL );

		if ( doAddSelf )
		{
			AddNode( renderableTransforms, quadAABB, maxPatchSize, cameraViewProjTransform, completelyInFrustum, !removeBL, !removeBR, !removeTR, !removeTL );

			return QuadNodeCullStatus::Added;
		}

		if ( childrenAdded[ 0 ] == QuadNodeCullStatus::Added || childrenAdded[ 1 ] == QuadNodeCullStatus::Added || childrenAdded[ 2 ] == QuadNodeCullStatus::Added || childrenAdded[ 3 ] == QuadNodeCullStatus::Added )
			return QuadNodeCullStatus::Added;
		else
			return QuadNodeCullStatus::OutOfFrustum;
	}
}
