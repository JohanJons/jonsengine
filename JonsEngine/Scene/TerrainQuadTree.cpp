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
		
		uint32_t rootLODLevel = 0;
		CreateGridNode( width / 2, height / 2, width, height, rootLODLevel );
		QuadNodeAABB& quadAABB = mGridTraversal.back();
		ProcessQuadNode( quadAABB, heightmapData, width, rootLODLevel );

		// translate negative width&height / 2 to center it at origo
		Mat4 finalTransform = glm::translate( -Vec3( width / 2, 0, height / 2) );
		finalTransform = worldTransform * finalTransform;

		for ( QuadNodeAABB& node : mGridTraversal )
			node.mAABB = node.mAABB * finalTransform;

		assert( ExpectedNumNodes( width, mPatchMinSize ) == GetNumNodes() );
	}

	void TerrainQuadTree::CullNodes( std::vector<Mat4>& nodeTransforms, std::vector<float>& LODRanges, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const
	{
		nodeTransforms.clear();
		LODRanges.clear();

		CalculateLODRanges( LODRanges, zNear, zFar );

		CullQuad( nodeTransforms, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );
	}

	uint32_t TerrainQuadTree::GetNumLODRanges() const
	{
		return static_cast<uint32_t>( std::log( 3 * GetNumNodes() + 1 ) / std::log( 4 ) );
	}

	void TerrainQuadTree::GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const
	{
		const QuadNodeAABB& toplevelNode = mGridTraversal.front();
		Vec3 minAABB = toplevelNode.mAABB.Min(), maxAABB = toplevelNode.mAABB.Max();

		worldMin = Vec2( minAABB.x, minAABB.z );
		worldMax = Vec2( maxAABB.x, maxAABB.z );
	}

	void TerrainQuadTree::AddNode( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB ) const
	{
		Vec3 center = quadAABB.mAABB.GetCenter();
		Vec3 extent = quadAABB.mAABB.GetExtent();

		Mat4 transform = glm::translate( Vec3( center.x, 0.0f, center.z ) );
		transform = glm::scale( transform, extent * 2.0f );
		nodes.emplace_back( transform );
	}

	bool TerrainQuadTree::CullQuad( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const
	{
		// if parent is fully in frustum, so are we
		AABBIntersection intersection = parentFullyInFrustum ? AABBIntersection::Inside : Intersection( quadAABB.mAABB, cameraViewProjTransform );
		if ( intersection == AABBIntersection::Outside )
			return false;

		float distanceLimit = LODRanges.at( quadAABB.mLODIndex );
		float cameraToQuadDistance = glm::distance( cameraWorldPos, quadAABB.mAABB.GetCenter() );
		bool isWithinRange = cameraToQuadDistance <= distanceLimit;
		if ( !isWithinRange )
			return false;
		
		uint32_t childLODIndex = quadAABB.mLODIndex + 1;
		bool isValidLODIndex = childLODIndex < LODRanges.size();
		if ( !isValidLODIndex )
		{
			AddNode( nodes, quadAABB );
			return true;
		}

		float nextDistanceLimit = LODRanges.at( childLODIndex );
		bool anyChildIsWithNextRange = false;
		for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			float childCameraToQuadDistance = glm::distance( cameraWorldPos, childQuad.mAABB.GetCenter() );
			if ( childCameraToQuadDistance <= nextDistanceLimit )
			{
				anyChildIsWithNextRange = true;
				break;
			}
		}

		if ( !anyChildIsWithNextRange )
		{
			AddNode( nodes, quadAABB );
			return true;
		}

		/*bool nextIsWithinRange = cameraToQuadDistance <= nextDistanceLimit;
		if ( !nextIsWithinRange )
		{
			AddNode( nodes, quadAABB );
			return true;
		}*/

		bool completelyInFrustum = intersection == AABBIntersection::Inside;
		std::array<bool, QuadNodeAABB::NUM_CHILDREN> childrenAdded;
		for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			childrenAdded[ childIndex ] = CullQuad( nodes, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges, completelyInFrustum );

			if ( !childrenAdded[ childIndex ] )
			{
				// TODO: remove & alter tess level
				AddNode( nodes, childQuad );
			}
		}

		return true;

		/*for ( bool added : childrenAdded )
		{
			if ( !added )
			{

			}
		}*/

		//bool allWithinRange = true;
		/*for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			float childModeDistance = LODRanges.at( childQuad.mLODIndex );
			float childCameraToQuadDistance = glm::distance( cameraWorldPos, childQuad.mAABB.GetCenter() );
			//allWithinRange &= childCameraToQuadDistance <= childModeDistance;
		}*/

		/*if ( allWithinRange )
		{
			for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
			{
				const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
				CullQuad( nodes, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges );
			}
		}
		else
		{
			AddNode( nodes, quadAABB );
		}*/

		/*float nodeDistance = LODRanges.at( quadAABB.mLODIndex );
		float cameraToQuadDistance = glm::distance( cameraWorldPos, quadAABB.mAABB.GetCenter() );
		bool isWithinRange = cameraToQuadDistance <= nodeDistance;
		isWithinRange = isWithinRange;

		for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			nodeDistance = LODRanges.at( childQuad.mLODIndex );
			cameraToQuadDistance = glm::distance( cameraWorldPos, childQuad.mAABB.GetCenter() );
			bool isWithinRange2 = cameraToQuadDistance <= nodeDistance;
			isWithinRange2 = isWithinRange2;
		}

		uint32_t childLODIndex = quadAABB.mLODIndex + 1;
		bool isValidLODIndex = childLODIndex < LODRanges.size();
		if ( isValidLODIndex )
		{

		}*/

		/*if ( intersection == AABBIntersection::Partial || intersection == AABBIntersection::Inside )
		{
			if ( !quadAABB.mChildBegin )
			{
				AddNode( nodes, quadAABB );
			}
			else
			{
				for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
				{
					const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
					CullQuad( nodes, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges );
				}

				uint32_t childLODIndex = quadAABB.mLODIndex + 1;
				bool isValidChildLODIndex = childLODIndex < LODRanges.size();
				if ( isValidChildLODIndex )
				{
					float nodeDistance = LODRanges.at( childLODIndex );
					float cameraToQuadDistance = glm::distance( cameraWorldPos, quadAABB.mAABB.GetCenter() );
					bool isWithinRange = cameraToQuadDistance <= nodeDistance;
				}
			}


			else
			{
				for ( uint32_t childIndex = QuadNodeAABB::ChildOffset::TOP_LEFT; childIndex < QuadNodeAABB::ChildOffset::NUM_CHILDREN; ++childIndex )
				{
					const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
					CullQuad( nodes, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges );
				}
			}
		}
		//else if ( intersection == AABBIntersection::Inside )
		//{
		//	AddNode( nodes, quadAABB );
		//}*/
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

	void TerrainQuadTree::CreateGridNode( uint32_t centerX, uint32_t centerZ, uint32_t width, uint32_t height, uint32_t LODlevel )
	{
		uint32_t extentX = width / 2;
		uint32_t extentZ = height / 2;

		Vec3 min( centerX - extentX, 0, centerZ - extentZ );
		Vec3 max( centerX + extentX, 0, centerZ + extentZ );

		mGridTraversal.emplace_back( min, max, LODlevel );
	}

	void TerrainQuadTree::ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth, uint32_t LODlevel )
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
		uint32_t childNodeLODLevel = LODlevel + 1;

		// TL - TR - BL - BR
		CreateGridNode( centerX - halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel );
		quadAABB.mChildBegin = &mGridTraversal.back();
		CreateGridNode( centerX + halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel );
		CreateGridNode( centerX + halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel );
		CreateGridNode( centerX - halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel );

		// process children and collect min/max Y for AABB
		for ( uint32_t offset = QuadNodeAABB::TOP_LEFT; offset < QuadNodeAABB::NUM_CHILDREN; ++offset )
		{
			QuadNodeAABB& childNode = *( quadAABB.mChildBegin + offset );
			ProcessQuadNode( childNode, heightmapData, heightmapWidth, childNodeLODLevel );

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
}

