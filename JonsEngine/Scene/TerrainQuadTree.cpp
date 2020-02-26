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

	float GetHeightVal( const std::vector<uint8_t>& heightmapData, TextureType heightmapType, uint32_t index )
	{
		switch ( heightmapType )
		{
			case TextureType::Height8:
			{
				uint8_t val = heightmapData.at( index );
				return Normalize( val );
			}
			case TextureType::Height16:
			{
				uint32_t twoByteIndex = index * 2;
				const uint8_t* dat = &heightmapData.at( twoByteIndex );
				uint16_t val = *( reinterpret_cast<const uint16_t*>( dat ) );
				return Normalize( val );
			}
			default:
			{
				assert( false && "Bad texture type provided to quad tree" );
				return 0.0f;
			}
		}
	}
}

namespace JonsEngine
{
	TerrainQuadTree::TerrainQuadTree( const std::vector<uint8_t>& heightmapData, TextureType heightmapType, uint32_t width, uint32_t height, uint32_t patchMinSize, uint32_t patchMaxSize, float heightmapScale, const Mat4& worldTransform ) :
		mPatchMinSize( patchMinSize ),
		mPatchMaxSize( patchMaxSize ),
		mHeightmapScale( heightmapScale )
	{
		float fWidth = static_cast<float>( width );
		float fHeight = static_cast<float>( height );

		assert( patchMinSize && fWidth >= 0.0f && fHeight >= 0.0f && !heightmapData.empty() );
		assert( heightmapType == TextureType::Height8 || heightmapType == TextureType::Height16 );

		uint32_t numNodes = ExpectedNumNodes( fWidth, mPatchMinSize );
		mGridTraversal.reserve( numNodes );
		
		float yTranslation = worldTransform[ 3 ].y;
		uint32_t rootTreeLevel = 0;
		CreateGridNode( nullptr, fWidth / 2, fHeight / 2, fWidth, fHeight, rootTreeLevel, yTranslation );
		QuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB, heightmapData, heightmapType, width, rootTreeLevel, yTranslation );

		// translate negative width&height / 2 to center it at origo
		Mat4 finalTransform = glm::translate( -Vec3( fWidth / 2, 0, fHeight / 2) );
		finalTransform = worldTransform * finalTransform;

		for ( QuadNodeAABB& node : mGridTraversal )
			node.mFrustumAABB = node.mFrustumAABB * finalTransform;

		assert( numNodes == GetNumNodes() );
	}

	void TerrainQuadTree::CullNodes( QuadTreeCullData& outData, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const
	{
		const QuadNodeAABB& rootNode = mGridTraversal.front();
		bool isTooLargePatch = GetPatchSize( rootNode ) > mPatchMaxSize;

		CalculateLODRanges( outData, zNear, zFar );
		CullQuad( outData, rootNode, cameraWorldPos, cameraViewProjTransform, false, isTooLargePatch );

#if _DEBUG
		assert( ValidateCulledNodes( outData.mTransforms ) ); 
#endif
	}

	uint32_t TerrainQuadTree::GetNumLODLevels() const
	{
		uint32_t numLODRanges = static_cast<uint32_t>( log2( mPatchMaxSize ) - log2( mPatchMinSize ) ) + 1;
		return numLODRanges;
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

	void TerrainQuadTree::CreateGridNode( QuadNodeAABB* parent, float centerX, float centerZ, float width, float height, uint32_t treeLevel, float yTranslation )
	{
		float extentX = width / 2;
		float extentZ = height / 2;

		float minX = centerX - extentX, maxX = centerX + extentX;
		float minZ = centerZ - extentZ, maxZ = centerZ + extentZ;

		Vec3 frustumMin( minX, 0, minZ );
		Vec3 frustumMax( maxX, 0, maxZ );

		mGridTraversal.emplace_back( frustumMin, frustumMax, parent, treeLevel );
	}

	void TerrainQuadTree::ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, TextureType heightmapType, uint32_t heightmapWidth, uint32_t treeLevel, float yTranslation )
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
					float val = GetHeightVal( heightmapData, heightmapType, index );
					val *= mHeightmapScale;

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
		uint32_t childNodeTreeLevel = treeLevel + 1;

		// BL - BR - TR - TL
		// TODO: validate
		CreateGridNode( &quadAABB, centerX - halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeTreeLevel, yTranslation );
		quadAABB.mChildBegin = &mGridTraversal.back();
		CreateGridNode( &quadAABB, centerX + halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeTreeLevel, yTranslation );
		CreateGridNode( &quadAABB, centerX + halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeTreeLevel, yTranslation );
		CreateGridNode( &quadAABB, centerX - halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeTreeLevel, yTranslation );

		// process children and collect min/max Y for AABB
		for ( uint32_t childIndex = QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT; childIndex < QuadChildEnum::QUAD_CHILD_COUNT; ++childIndex )
		{
			QuadNodeAABB& childNode = *( quadAABB.mChildBegin + childIndex );
			ProcessQuadNode( childNode, heightmapData, heightmapType, heightmapWidth, childNodeTreeLevel, yTranslation );

			minY = std::min( minY, childNode.mFrustumAABB.Min().y );
			maxY = std::max( maxY, childNode.mFrustumAABB.Max().y );
		}

		min.y = minY;
		max.y = maxY;
		quadAABB.mFrustumAABB = AABB( min, max );
	}

	uint32_t TerrainQuadTree::GetPatchSize( const QuadNodeAABB& quadAABB ) const
	{
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();
		uint32_t patchSize = static_cast<uint32_t>( extent.x * 2.0f );
		patchSize = std::max( patchSize, static_cast<uint32_t>( extent.z * 2.0f ) );

		return patchSize;
	}

	uint32_t TerrainQuadTree::GetLODLevel( const QuadNodeAABB& quadAABB ) const
	{
		uint32_t LOD = 0;
		uint32_t patchSize = GetPatchSize( quadAABB );
		if ( patchSize >= mPatchMaxSize )
		{
			return LOD;
		}

		uint32_t numLODs = GetNumLODLevels();
		assert( patchSize >= mPatchMinSize );
		uint32_t diff = static_cast<uint32_t>( log2( patchSize ) - log2( mPatchMinSize ) ) + 1;
		assert( diff <= numLODs );
		LOD = numLODs - diff;

		return LOD;
	}

	void TerrainQuadTree::CalculateLODRanges( QuadTreeCullData& outData, float zNear, float zFar ) const
	{
		uint32_t numLODRanges = GetNumLODLevels();

		outData.mLODRanges.resize( numLODRanges );
		outData.mMorphConstants.resize( numLODRanges );

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

			outData.mLODRanges[ index ] = prevPos + sect * currentDetailBalance;
			prevPos = outData.mLODRanges[ index ];
			currentDetailBalance *= LODDistanceRatio;
		}

		prevPos = zNear;
		for ( uint32_t i = 0; i < numLODRanges; ++i )
		{
			uint32_t index = numLODRanges - i - 1;

			float morphEnd = outData.mLODRanges[ index ];
			float morphStart = prevPos + ( morphEnd - prevPos ) * gMorphStartRatio;

			prevPos = morphEnd;

			const float errorFudge = 0.01f;
			morphEnd = glm::lerp( morphEnd, morphStart, errorFudge );
			
			outData.mMorphConstants[ index ].x = morphEnd / ( morphEnd - morphStart );
			outData.mMorphConstants[ index ].y = 1.0f / ( morphEnd - morphStart );
		}
	}

	void TerrainQuadTree::AddNode( QuadTreeCullData& outData, const QuadNodeAABB& quadAABB, uint32_t LODLevel, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();

		assert( extent.x * 2.0f <= mPatchMaxSize && extent.z * 2.0f <= mPatchMaxSize );

		Mat4 transform = glm::translate( Vec3( center.x, 0.0f, center.z ) );
		transform = glm::scale( transform, extent * 2.0f );

		outData.mTransforms.emplace_back( transform );
		outData.mLODs.emplace_back( LODLevel );
	}

	void TerrainQuadTree::AddNode( QuadTreeCullData& outData, const QuadNodeAABB& quadAABB, uint32_t LODLevel, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum, bool addBL, bool addBR, bool addTR, bool addTL ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();
		Vec3 halfExtent = extent / 2.0f;

		if ( addBL )
		{
			const QuadNodeAABB& childAABB = *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT );
			uint32_t childLOD = GetLODLevel( childAABB );
			AddNode( outData, childAABB, childLOD, cameraViewProjTransform, parentFullyInFrustum );
		}

		if ( addBR )
		{
			const QuadNodeAABB& childAABB = *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_BOTTOM_RIGHT );
			uint32_t childLOD = GetLODLevel( childAABB );
			AddNode( outData, childAABB, childLOD, cameraViewProjTransform, parentFullyInFrustum );
		}

		if ( addTR )
		{
			const QuadNodeAABB& childAABB = *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_TOP_RIGHT );
			uint32_t childLOD = GetLODLevel( childAABB );
			AddNode( outData, childAABB, childLOD, cameraViewProjTransform, parentFullyInFrustum );
		}

		if ( addTL )
		{
			const QuadNodeAABB& childAABB = *( quadAABB.mChildBegin + QuadChildEnum::QUAD_CHILD_TOP_LEFT );
			uint32_t childLOD = GetLODLevel( childAABB );
			AddNode( outData, childAABB, childLOD, cameraViewProjTransform, parentFullyInFrustum );
		}
	}

	QuadNodeCullStatus TerrainQuadTree::CullQuad( QuadTreeCullData& outData, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum, bool parentTooLargePatch ) const
	{
		// if parent is fully in frustum, so are we
		AABBIntersection frustumIntersection = parentFullyInFrustum ? AABBIntersection::Inside : Intersection( quadAABB.mFrustumAABB, cameraViewProjTransform );
		if ( frustumIntersection == AABBIntersection::Outside )
			return QuadNodeCullStatus::OutOfFrustum;

		uint32_t LODLevel = GetLODLevel( quadAABB );

		float distanceLimit = outData.mLODRanges.at( LODLevel );
		AABBIntersection intersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, distanceLimit );
		bool isWithinDistance = intersection == AABBIntersection::Partial || intersection == AABBIntersection::Inside;
		if ( !parentTooLargePatch && !isWithinDistance )
			return QuadNodeCullStatus::OutOfRange;

		bool completelyInFrustum = frustumIntersection == AABBIntersection::Inside;

		// last LOD
		uint32_t nextLODIndex = LODLevel + 1;
		bool isValidLODIndex = nextLODIndex < outData.mLODRanges.size();
		if ( !isValidLODIndex )
		{
			AddNode( outData, quadAABB, LODLevel, cameraViewProjTransform, completelyInFrustum );
			return QuadNodeCullStatus::Added;
		}

		bool isTooLargePatch = GetPatchSize( quadAABB ) > mPatchMaxSize;

		float nextDistanceLimit = outData.mLODRanges.at( nextLODIndex );
		AABBIntersection nextIntersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, nextDistanceLimit );
		bool nextIsWithinRange = nextIntersection == AABBIntersection::Partial || nextIntersection == AABBIntersection::Inside;
		if ( !isTooLargePatch && !nextIsWithinRange )
		{
			AddNode( outData, quadAABB, LODLevel, cameraViewProjTransform, completelyInFrustum );
			return QuadNodeCullStatus::Added;
		}

		std::array<QuadNodeCullStatus, QuadChildEnum::QUAD_CHILD_COUNT> childrenAdded{ QuadNodeCullStatus::Undefined, QuadNodeCullStatus::Undefined, QuadNodeCullStatus::Undefined, QuadNodeCullStatus::Undefined };
		for ( uint32_t childIndex = QuadChildEnum::QUAD_CHILD_BOTTOM_LEFT; childIndex < QuadChildEnum::QUAD_CHILD_COUNT; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			childrenAdded[ childIndex ] = CullQuad( outData, childQuad, cameraWorldPos, cameraViewProjTransform, completelyInFrustum, isTooLargePatch );
		}

		// remove if childnodes already selected, or out of frustum
		bool removeBL = childrenAdded[ 0 ] == QuadNodeCullStatus::Added || childrenAdded[ 0 ] == QuadNodeCullStatus::OutOfFrustum;
		bool removeBR = childrenAdded[ 1 ] == QuadNodeCullStatus::Added || childrenAdded[ 1 ] == QuadNodeCullStatus::OutOfFrustum;
		bool removeTR = childrenAdded[ 2 ] == QuadNodeCullStatus::Added || childrenAdded[ 2 ] == QuadNodeCullStatus::OutOfFrustum;
		bool removeTL = childrenAdded[ 3 ] == QuadNodeCullStatus::Added || childrenAdded[ 3 ] == QuadNodeCullStatus::OutOfFrustum;
		bool doAddSelf = !( removeBL && removeBR && removeTR && removeTL );

		if ( doAddSelf )
		{
			AddNode( outData, quadAABB, LODLevel, cameraViewProjTransform, completelyInFrustum, !removeBL, !removeBR, !removeTR, !removeTL );

			return QuadNodeCullStatus::Added;
		}

		if ( childrenAdded[ 0 ] == QuadNodeCullStatus::Added || childrenAdded[ 1 ] == QuadNodeCullStatus::Added || childrenAdded[ 2 ] == QuadNodeCullStatus::Added || childrenAdded[ 3 ] == QuadNodeCullStatus::Added )
			return QuadNodeCullStatus::Added;
		else
			return QuadNodeCullStatus::OutOfFrustum;
	}
}
