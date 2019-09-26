#include "Scene/TerrainQuadTree.h"

#include "Core/Math/Intersection.h"
#include "Core/Math/MathUtils.h"

namespace JonsEngine
{
	void GetTransformExtentsXZ( const Mat4& transform, Vec2& min, Vec2& max )
	{
		const Vec4& translation = transform[ 3 ];
		float xExtent = transform[ 0 ].x / 2;
		float zExtent = transform[ 2 ].z / 2;

		min = Vec2( translation.x - xExtent, translation.z - zExtent );
		max = Vec2( translation.x + xExtent, translation.z + zExtent );
	}

	TerrainQuadTree::TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform ) :
		mPatchMinSize( patchMinSize ),
		mHeightmapScale( heightmapScale )
	{
		assert( patchMinSize && width && height && !heightmapData.empty() );

		mGridTraversal.reserve( ExpectedNumNodes( width, mPatchMinSize ) );
		
		float yTranslation = worldTransform[ 3 ].y;
		uint32_t rootLODLevel = 0;
		CreateGridNode( width / 2, height / 2, width, height, rootLODLevel, yTranslation );
		QuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB, heightmapData, width, rootLODLevel, yTranslation );

		// translate negative width&height / 2 to center it at origo
		Mat4 finalTransform = glm::translate( -Vec3( width / 2, 0, height / 2) );
		finalTransform = worldTransform * finalTransform;

		for ( QuadNodeAABB& node : mGridTraversal )
			node.mFrustumAABB = node.mFrustumAABB * finalTransform;

		assert( ExpectedNumNodes( width, mPatchMinSize ) == GetNumNodes() );
	}

	void TerrainQuadTree::CullNodes( std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult, std::vector<float>& LODRanges, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const
	{
		nodeTransforms.clear();
		LODRanges.clear();

		CalculateLODRanges( LODRanges, zNear, zFar );

		CullQuad( nodeTransforms, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );

		CalculateTessellationFactors( nodeTransforms, tessEdgeMult );
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

	void TerrainQuadTree::AddNode( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const std::vector<float>& LODRanges ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();

		Mat4 transform = glm::translate( Vec3( center.x, 0.0f, center.z ) );
		transform = glm::scale( transform, extent * 2.0f );
		nodes.emplace_back( transform );
	}

	TerrainQuadTree::CullStatus TerrainQuadTree::CullQuad( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const
	{
		// if parent is fully in frustum, so are we
		AABBIntersection frustumIntersection = parentFullyInFrustum ? AABBIntersection::Inside : Intersection( quadAABB.mFrustumAABB, cameraViewProjTransform );
		if ( frustumIntersection == AABBIntersection::Outside )
			return CullStatus::OutOfFrustum;

		float distanceLimit = LODRanges.at( quadAABB.mLODIndex );
		AABBIntersection intersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, distanceLimit );
		bool isWithinDistance = intersection == AABBIntersection::Partial || intersection == AABBIntersection::Inside;
		if ( !isWithinDistance )
			return CullStatus::OutOfRange;
		
		// last LOD
		uint32_t nextLODIndex = quadAABB.mLODIndex + 1;
		bool isValidLODIndex = nextLODIndex < LODRanges.size();
		if ( !isValidLODIndex )
		{
			AddNode( nodes, quadAABB, cameraWorldPos, LODRanges );
			return CullStatus::Added;
		}

		float nextDistanceLimit = LODRanges.at( nextLODIndex );
		AABBIntersection nextIntersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, nextDistanceLimit );
		bool nextIsWithinRange = nextIntersection == AABBIntersection::Partial || nextIntersection == AABBIntersection::Inside;
		if ( !nextIsWithinRange )
		{
			AddNode( nodes, quadAABB, cameraWorldPos, LODRanges );
			return CullStatus::Added;
		}

		CullStatus Status = CullStatus::OutOfFrustum;
		bool completelyInFrustum = frustumIntersection == AABBIntersection::Inside;
		std::array<CullStatus, QuadNodeAABB::NUM_CHILDREN> childrenAdded;
		for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			childrenAdded[ childIndex ] = CullQuad( nodes, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges, completelyInFrustum );

			if ( childrenAdded[ childIndex ] == CullStatus::OutOfRange )
			{
				// TODO: remove & alter tess level
				AddNode( nodes, childQuad, cameraWorldPos, LODRanges );
				Status = CullStatus::Added;
			}
		}

		return Status;
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

	void TerrainQuadTree::CreateGridNode( uint32_t centerX, uint32_t centerZ, uint32_t width, uint32_t height, uint32_t LODlevel, float yTranslation )
	{
		uint32_t extentX = width / 2;
		uint32_t extentZ = height / 2;

		uint32_t minX = centerX - extentX, maxX = centerX + extentX;
		uint32_t minZ = centerZ - extentZ, maxZ = centerZ + extentZ;

		Vec3 frustumMin( minX, 0, minZ );
		Vec3 frustumMax( maxX, 0, maxZ );

		mGridTraversal.emplace_back( frustumMin, frustumMax, LODlevel );
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

		uint32_t centerX = static_cast<uint32_t>( center.x ), centerZ = static_cast<uint32_t>( center.z );
		uint32_t childWidth = static_cast<uint32_t>( extent.x );
		uint32_t childHeight = static_cast<uint32_t>( extent.z );
		uint32_t halfChildWidth = childWidth / 2;
		uint32_t halfChildHeight = childHeight / 2;
		uint32_t childNodeLODLevel = LODlevel + 1;

		// TL - TR - BL - BR
		CreateGridNode( centerX - halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		quadAABB.mChildBegin = &mGridTraversal.back();
		CreateGridNode( centerX + halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		CreateGridNode( centerX + halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		CreateGridNode( centerX - halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );

		// process children and collect min/max Y for AABB
		for ( uint32_t offset = QuadNodeAABB::TOP_LEFT; offset < QuadNodeAABB::NUM_CHILDREN; ++offset )
		{
			QuadNodeAABB& childNode = *( quadAABB.mChildBegin + offset );
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

	void TerrainQuadTree::CalculateTessellationFactors( std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult ) const
	{
		tessEdgeMult.resize( nodeTransforms.size(), Vec4( 1.0f ) );

		int32_t numNodes = static_cast<int32_t>( nodeTransforms.size() );
		for ( int32_t outerIndex = numNodes - 1; outerIndex >= 0; --outerIndex )
		{
			Mat4& transform = nodeTransforms[ outerIndex ];

			Vec2 transformMin, transformMax;
			GetTransformExtentsXZ( transform, transformMin, transformMax );

			float midX = ( transformMin.x + transformMax.x ) / 2;
			float midZ = ( transformMin.y + transformMax.y ) / 2;
			Vec2 top( midX, transformMax.y ), left( transformMin.x, midZ ), bottom( midX, transformMin.y ), right( transformMax.x, midZ );

			for ( int32_t innerIndex = outerIndex - 1; innerIndex >= 0; --innerIndex )
			{
				Mat4& otherTransform = nodeTransforms[ innerIndex ];
				// order: left - bottom - right - top
				Vec4& otherTessMult = tessEdgeMult[ innerIndex ];

				Vec2 otherMin, otherMax;
				GetTransformExtentsXZ( otherTransform, otherMin, otherMax );

				if ( left.x == otherMax.x && ( left.y == otherMin.y || left.y == otherMax.y ) )
				{
					float otherLength = otherMax.y - otherMin.y;
					float thislength = transformMax.y - transformMin.y;
					if ( thislength / otherLength >= 4.0f )
					{
						otherTransform[ 0 ][ 0 ] *= 0.5f;
						otherTransform[ 3 ][ 0 ] -= otherTransform[ 0 ][ 0 ];
					}

					/*Vec3 scale( otherTransform[ 0 ][ 0 ] );
					scale.z /= 2.0f;
					Mat4 scaleTransform = glm::scale( scale );

					Vec3 translation( otherTransform[ 3 ][ 0 ] ) ;
					translation.x += ( otherTransform[ 0 ][ 0 ] * 2.0f );

					Vec3 translation1( translation );


					Mat4 translation1 = glm::translate( )


					Mat4 transform( 1.0f );

					nodeTransforms.emplace_back();*/
					otherTessMult.x = 2.0f;
				}
				// bottom-edge
				else if ( bottom.y == otherMax.y && ( bottom.x == otherMin.x || bottom.x == otherMax.x ) )
				{
					float otherLength = otherMax.x - otherMin.x;
					float thislength = transformMax.x - transformMin.x;
					if ( thislength / otherLength >= 4.0f )
					{
						otherTransform[ 2 ][ 2 ] *= 0.5f;
						otherTransform[ 3 ][ 2 ] -= otherTransform[ 2 ][ 2 ];
					}

					otherTessMult.y = 2.0f;
				}
				// right-edge
				else if ( right.x == otherMin.x && ( right.y == otherMin.y || right.y == otherMax.y ) )
				{
					float otherLength = otherMax.y - otherMin.y;
					float thislength = transformMax.y - transformMin.y;
					if ( thislength / otherLength >= 4.0f )
					{
						otherTransform[ 0 ][ 0 ] *= 0.5f;
						otherTransform[ 3 ][ 0 ] += otherTransform[ 2 ][ 2 ];
					}

					otherTessMult.z = 2.0f;
				}
				// top-edge
				else if ( top.y == otherMin.y && ( top.x == otherMin.x || top.x == otherMax.x ) )
				{
					float otherLength = otherMax.x - otherMin.x;
					float thislength = transformMax.x - transformMin.x;
					if ( thislength / otherLength >= 4.0f )
					{
						otherTransform[ 2 ][ 2 ] *= 0.5f;
						otherTransform[ 3 ][ 2 ] += otherTransform[ 2 ][ 2 ];
					}

					otherTessMult.y = 2.0f;
				}
			}
		}




		for ( const Mat4& transform : nodeTransforms )
		{
			Vec2 transformMin, transformMax;
			GetTransformExtentsXZ( transform, transformMin, transformMax );

			float midX = ( transformMin.x + transformMax.x ) / 2;
			float midZ = ( transformMin.y + transformMax.y ) / 2;
			Vec2 top( midX, transformMax.y ), left( transformMin.x, midZ ), bottom( midX, transformMin.y ), right( transformMax.x, midZ );

			// DEBUG
			// DEBUG
			// DEBUG
			// DEBUG
			// DEBUG
			for ( Mat4& otherTransform : nodeTransforms )
			{
				if ( &transform == &otherTransform )
				{
					continue;
				}

				Vec2 otherMin, otherMax;
				GetTransformExtentsXZ( otherTransform, otherMin, otherMax );

				// left-edge
				if ( left.x == otherMax.x && ( left.y == otherMin.y || left.y == otherMax.y ) )
				{
					float otherLength = otherMax.y - otherMin.y;
					float thislength = transformMax.y - transformMin.y;
					assert( thislength / otherLength < 4.0f );
				}
				// bottom-edge
				else if ( bottom.y == otherMax.y && ( bottom.x == otherMin.x || bottom.x == otherMax.x ) )
				{
					float otherLength = otherMax.x - otherMin.x;
					float thislength = transformMax.x - transformMin.x;
					assert( thislength / otherLength < 4.0f );
				}
				// right-edge
				else if ( right.x == otherMin.x && ( right.y == otherMin.y || right.y == otherMax.y ) )
				{
					float otherLength = otherMax.y - otherMin.y;
					float thislength = transformMax.y - transformMin.y;
					assert( thislength / otherLength < 4.0f );
				}
				// top-edge
				else if ( top.y == otherMin.y && ( top.x == otherMin.x || top.x == otherMax.x ) )
				{
					float otherLength = otherMax.x - otherMin.x;
					float thislength = transformMax.x - transformMin.x;
					assert( thislength / otherLength < 4.0f );
				}
			}
		}
	}
}
