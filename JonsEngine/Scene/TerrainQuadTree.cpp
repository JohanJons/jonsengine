#include "Scene/TerrainQuadTree.h"

#include "Core/Math/Intersection.h"
#include "Core/Math/MathUtils.h"

#include <set>

namespace
{
	using namespace JonsEngine;

	using ChildNeighbours = std::array<QuadNodeAABB::ChildNode, 4>;
	constexpr std::array<QuadNodeAABB::ChildNode, 4> TLNeighbours{ QuadNodeAABB::INVALID, QuadNodeAABB::BOTTOM_LEFT, QuadNodeAABB::TOP_RIGHT, QuadNodeAABB::INVALID };
	constexpr std::array<QuadNodeAABB::ChildNode, 4> BLNeighbours{ QuadNodeAABB::INVALID, QuadNodeAABB::INVALID, QuadNodeAABB::BOTTOM_RIGHT, QuadNodeAABB::TOP_LEFT };
	constexpr std::array<QuadNodeAABB::ChildNode, 4> BRNeighbours{ QuadNodeAABB::BOTTOM_LEFT, QuadNodeAABB::INVALID, QuadNodeAABB::INVALID, QuadNodeAABB::TOP_RIGHT };
	constexpr std::array<QuadNodeAABB::ChildNode, 4> TRNeighbours{ QuadNodeAABB::TOP_LEFT, QuadNodeAABB::BOTTOM_RIGHT, QuadNodeAABB::INVALID, QuadNodeAABB::INVALID };
	constexpr std::array<ChildNeighbours, 4> ChildNeighbourData{ TLNeighbours, BLNeighbours, BRNeighbours, TRNeighbours };

	void PopNeighbouringChildren( QuadNodeAABB::ChildNode& first, QuadNodeAABB::ChildNode& second, std::vector<QuadNodeAABB::ChildNode>& childNodes )
	{
		first = QuadNodeAABB::INVALID;
		second = QuadNodeAABB::INVALID;

		if ( childNodes.empty() )
		{
			return;
		}

		first = childNodes.back();
		childNodes.pop_back();
		if ( childNodes.empty() )
		{
			return;
		}

		for ( int32_t index = 0; index < static_cast<int32_t>( childNodes.size() ); ++index )
		{
			QuadNodeAABB::ChildNode other = childNodes[ index ];

			// only direct neighbours
			if ( other - first % 2 != 0 )
			{
				second = other;
				childNodes.erase( childNodes.begin() + index );
				break;
			}
		}

		assert( first != QuadNodeAABB::INVALID && second != QuadNodeAABB::INVALID );
	}

	QuadNodeAABB::ChildNode GetOpposite( QuadNodeAABB::ChildNode child, QuadNodeNeighbours::Facing facing )
	{
		switch ( facing )
		{
			case QuadNodeNeighbours::RIGHT:
			case QuadNodeNeighbours::LEFT:
			{
				switch ( child )
				{
					case QuadNodeAABB::BOTTOM_LEFT: return QuadNodeAABB::BOTTOM_RIGHT;
					case QuadNodeAABB::BOTTOM_RIGHT: return QuadNodeAABB::BOTTOM_LEFT;
					case QuadNodeAABB::TOP_LEFT: return QuadNodeAABB::TOP_RIGHT;
					case QuadNodeAABB::TOP_RIGHT: return QuadNodeAABB::TOP_LEFT;
					default: return QuadNodeAABB::NUM_CHILDREN;
				}
			}

			case QuadNodeNeighbours::TOP:
			case QuadNodeNeighbours::BOTTOM:
			{
				switch ( child )
				{
					case QuadNodeAABB::BOTTOM_LEFT: return QuadNodeAABB::TOP_LEFT;
					case QuadNodeAABB::BOTTOM_RIGHT: return QuadNodeAABB::TOP_RIGHT;
					case QuadNodeAABB::TOP_LEFT: return QuadNodeAABB::BOTTOM_LEFT;
					case QuadNodeAABB::TOP_RIGHT: return QuadNodeAABB::BOTTOM_RIGHT;
					default: return QuadNodeAABB::NUM_CHILDREN;
				}
			}
			default: return QuadNodeAABB::NUM_CHILDREN;
		}
	}

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
		mGridNeighbours.resize( numNodes );
		
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

		// precompute neighbours
		ComputeNeighbours();
		assert( ValidateNeighbours() );

		assert( numNodes == GetNumNodes() );
		assert( mGridTraversal.size() == mGridNeighbours.size() );
	}

	void TerrainQuadTree::CullNodes( std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult, std::vector<float>& LODRanges, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const
	{
		nodeTransforms.clear();
		LODRanges.clear();
		//tessEdgeMult.clear();

		CalculateLODRanges( LODRanges, zNear, zFar );

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

		// TL - BL - BR - TR
		CreateGridNode( &quadAABB, centerX - halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		quadAABB.mChildBegin = &mGridTraversal.back();
		CreateGridNode( &quadAABB, centerX - halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		CreateGridNode( &quadAABB, centerX + halfChildWidth, centerZ - halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );
		CreateGridNode( &quadAABB, centerX + halfChildWidth, centerZ + halfChildHeight, childWidth, childHeight, childNodeLODLevel, yTranslation );

		// process children and collect min/max Y for AABB
		for ( int32_t childIndex = QuadNodeAABB::TOP_LEFT; childIndex < QuadNodeAABB::NUM_CHILDREN; ++childIndex )
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

	void TerrainQuadTree::ComputeNeighbours()
	{
		std::vector<QuadNodeAABB*> sortedNodes;
		for ( QuadNodeAABB& node : mGridTraversal )
			sortedNodes.emplace_back( &node );

		auto cmpFunc = []( const QuadNodeAABB* A, const QuadNodeAABB* B ) { return A->mLODIndex < B->mLODIndex; };
		std::sort( sortedNodes.begin(), sortedNodes.end(), cmpFunc );

		for ( QuadNodeAABB* quadNode : sortedNodes )
		{
			if ( !quadNode->mChildBegin )
				continue;

			QuadNodeAABB* rootNode = &( *mGridTraversal.begin() );

			size_t quadNodeIndex = std::distance( rootNode, quadNode );
			QuadNodeNeighbours& parentNeighboursData = mGridNeighbours.at( quadNodeIndex );

			//QuadNodeNeighbours* parentNeighboursData = nullptr;
			//if ( quadNode->mParent )
			//{
			//	size_t parentNodeIndex = std::distance( rootNode, quadNode->mParent );
			//	parentNeighboursData = &mGridNeighbours.at( parentNodeIndex );
			//}

			const QuadNodeAABB* topLeftChild = quadNode->mChildBegin + QuadNodeAABB::TOP_LEFT;
			const QuadNodeAABB* bottomLeftChild = quadNode->mChildBegin + QuadNodeAABB::BOTTOM_LEFT;
			const QuadNodeAABB* bottomRightChild = quadNode->mChildBegin + QuadNodeAABB::BOTTOM_RIGHT;
			const QuadNodeAABB* topRightChild = quadNode->mChildBegin + QuadNodeAABB::TOP_RIGHT;

			for ( int32_t childOffset = QuadNodeAABB::TOP_LEFT; childOffset < QuadNodeAABB::NUM_CHILDREN; ++childOffset )
			{
				QuadNodeAABB* currentChild = quadNode->mChildBegin + childOffset;

				size_t childQuadNodeIndex = std::distance( rootNode, currentChild );
				QuadNodeNeighbours& childNeighboursData = mGridNeighbours.at( childQuadNodeIndex );
				const ChildNeighbours& neighbourMapping = ChildNeighbourData[ childOffset ];

				for ( int32_t neighbourOffset = QuadNodeNeighbours::LEFT; neighbourOffset < QuadNodeNeighbours::NUM_OFFSETS; ++neighbourOffset )
				{
					QuadNodeAABB::ChildNode sibling = neighbourMapping[ neighbourOffset ];
					switch ( sibling )
					{
						case QuadNodeAABB::TOP_LEFT:
						{
							childNeighboursData.mSameLODNeighbours[ neighbourOffset ] = topLeftChild;
							break;
						}

						case QuadNodeAABB::BOTTOM_LEFT:
						{
							childNeighboursData.mSameLODNeighbours[ neighbourOffset ] = bottomLeftChild;
							break;
						}

						case QuadNodeAABB::BOTTOM_RIGHT:
						{
							childNeighboursData.mSameLODNeighbours[ neighbourOffset ] = bottomRightChild;
							break;
						}

						case QuadNodeAABB::TOP_RIGHT:
						{
							childNeighboursData.mSameLODNeighbours[ neighbourOffset ] = topRightChild;
							break;
						}

						default:
						{
							const QuadNodeAABB* parentNeighbour = parentNeighboursData.mSameLODNeighbours[ neighbourOffset ];
							if ( !parentNeighbour )
								continue;

							QuadNodeAABB::ChildNode opposite = GetOpposite( static_cast<QuadNodeAABB::ChildNode>( childOffset ), static_cast<QuadNodeNeighbours::Facing>( neighbourOffset ) );
							const QuadNodeAABB* neighbourChild = parentNeighbour->mChildBegin + opposite;
							assert( neighbourChild );

							childNeighboursData.mSameLODNeighbours[ neighbourOffset ] = neighbourChild;
							// must find manually!
							break;
						}
					}
				}
			}
		}
	}

	bool TerrainQuadTree::ValidateNeighbours() const
	{
		Vec2 worldMin, worldMax;
		GetWorldXZBounds( worldMin, worldMax );

		assert( mGridTraversal.size() == mGridNeighbours.size() );

		for ( int32_t index = 0; index < mGridTraversal.size(); ++index )
		{
			const QuadNodeAABB& nodeAABB = mGridTraversal[ index ];
			const QuadNodeNeighbours& nodeNeighbours = mGridNeighbours[ index ];

			Vec3 nodeMin = nodeAABB.mFrustumAABB.Min();
			Vec3 nodeMax = nodeAABB.mFrustumAABB.Max();

			const QuadNodeAABB* pLeftNeighbour = nodeNeighbours.mSameLODNeighbours[ QuadNodeNeighbours::LEFT ];
			if ( pLeftNeighbour )
			{
				Vec3 neighbourMax = pLeftNeighbour->mFrustumAABB.Max();
				if ( neighbourMax.x != nodeMin.x )
				{
					return false;
				}
			}
			else if ( nodeMin.x != worldMin.x )
			{
				return false;
			}

			const QuadNodeAABB* pBottomNeighbour = nodeNeighbours.mSameLODNeighbours[ QuadNodeNeighbours::BOTTOM ];
			if ( pBottomNeighbour )
			{
				Vec3 neighbourMax = pBottomNeighbour->mFrustumAABB.Max();
				if ( neighbourMax.z != nodeMin.z )
				{
					return false;
				}
			}
			else if ( nodeMin.z != worldMin.y )
			{
				return false;
			}

			const QuadNodeAABB* pRightNeighbour = nodeNeighbours.mSameLODNeighbours[ QuadNodeNeighbours::RIGHT ];
			if ( pRightNeighbour )
			{
				Vec3 neighbourMin = pRightNeighbour->mFrustumAABB.Min();
				if ( neighbourMin.x != nodeMax.x )
				{
					return false;
				}
			}
			else if ( nodeMax.x != worldMax.x )
			{
				return false;
			}

			const QuadNodeAABB* pTopNeighbour = nodeNeighbours.mSameLODNeighbours[ QuadNodeNeighbours::TOP ];
			if ( pTopNeighbour )
			{
				Vec3 neighbourMin = pTopNeighbour->mFrustumAABB.Min();
				if ( neighbourMin.z != nodeMax.z )
				{
					return false;
				}
			}
			else if ( nodeMax.z != worldMax.y )
			{
				return false;
			}
		}

		return true;
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

	void TerrainQuadTree::AddNode( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const std::vector<float>& LODRanges ) const
	{
		Vec3 center = quadAABB.mFrustumAABB.GetCenter();
		Vec3 extent = quadAABB.mFrustumAABB.GetExtent();

		Mat4 transform = glm::translate( Vec3( center.x, 0.0f, center.z ) );
		transform = glm::scale( transform, extent * 2.0f );
		nodes.emplace_back( transform );
	}

	QuadNodeCullStatus TerrainQuadTree::CullQuad( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const
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
			AddNode( nodes, quadAABB, cameraWorldPos, LODRanges );
			return QuadNodeCullStatus::Added;
		}

		float nextDistanceLimit = LODRanges.at( nextLODIndex );
		AABBIntersection nextIntersection = Intersection( quadAABB.mFrustumAABB, cameraWorldPos, nextDistanceLimit );
		bool nextIsWithinRange = nextIntersection == AABBIntersection::Partial || nextIntersection == AABBIntersection::Inside;
		if ( !nextIsWithinRange )
		{
			AddNode( nodes, quadAABB, cameraWorldPos, LODRanges );
			return QuadNodeCullStatus::Added;
		}

		QuadNodeCullStatus Status = QuadNodeCullStatus::OutOfFrustum;
		bool completelyInFrustum = frustumIntersection == AABBIntersection::Inside;
		std::array<QuadNodeCullStatus, QuadNodeAABB::NUM_CHILDREN> childrenAdded;
		for ( uint32_t childIndex = QuadNodeAABB::TOP_LEFT; childIndex < QuadNodeAABB::NUM_CHILDREN; ++childIndex )
		{
			const QuadNodeAABB& childQuad = *( quadAABB.mChildBegin + childIndex );
			childrenAdded[ childIndex ] = CullQuad( nodes, childQuad, cameraWorldPos, cameraViewProjTransform, LODRanges, completelyInFrustum );

			if ( childrenAdded[ childIndex ] == QuadNodeCullStatus::OutOfRange )
			{
				// TODO: remove & alter tess level
				//AddNode2( nodes, childQuad, cameraWorldPos, LODRanges );
				//Status = QuadNodeCullStatus::Added;
			}
		}

		if ( childrenAdded[ 0 ] == QuadNodeCullStatus::Added || childrenAdded[ 1 ] == QuadNodeCullStatus::Added || childrenAdded[ 2 ] == QuadNodeCullStatus::Added || childrenAdded[ 3 ] == QuadNodeCullStatus::Added )
		{
			Status = QuadNodeCullStatus::Added;
		}

		return Status;
	}
}
