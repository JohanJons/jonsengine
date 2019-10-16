#include "Scene/TerrainQuadTree.h"

#include "Core/Math/Intersection.h"
#include "Core/Math/MathUtils.h"

#include <set>

namespace JonsEngine
{
	using ChildNeighbours = std::array<QuadNodeAABB::ChildNode, 4>;
	constexpr std::array<QuadNodeAABB::ChildNode, 4> TLNeighbours{ QuadNodeAABB::INVALID, QuadNodeAABB::BOTTOM_LEFT, QuadNodeAABB::TOP_RIGHT, QuadNodeAABB::INVALID };
	constexpr std::array<QuadNodeAABB::ChildNode, 4> BLNeighbours{ QuadNodeAABB::INVALID, QuadNodeAABB::INVALID, QuadNodeAABB::BOTTOM_RIGHT, QuadNodeAABB::TOP_LEFT };
	constexpr std::array<QuadNodeAABB::ChildNode, 4> BRNeighbours{ QuadNodeAABB::BOTTOM_LEFT, QuadNodeAABB::INVALID, QuadNodeAABB::INVALID, QuadNodeAABB::TOP_RIGHT };
	constexpr std::array<QuadNodeAABB::ChildNode, 4> TRNeighbours{ QuadNodeAABB::TOP_LEFT, QuadNodeAABB::BOTTOM_RIGHT, QuadNodeAABB::INVALID, QuadNodeAABB::INVALID };
	constexpr std::array<ChildNeighbours, 4> ChildNeighbourData{ TLNeighbours, BLNeighbours, BRNeighbours, TRNeighbours };

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

		CalculateLODRanges( LODRanges, zNear, zFar );

		CullQuad( nodeTransforms, mGridTraversal.front(), cameraWorldPos, cameraViewProjTransform, LODRanges, false );

		CalculateTessellationFactors( nodeTransforms, tessEdgeMult );

		assert( nodeTransforms.size() == tessEdgeMult.size() );
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

		}

		return true;
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
		for ( int32_t childIndex = QuadNodeAABB::TOP_LEFT; childIndex < QuadNodeAABB::ChildNode::NUM_CHILDREN; ++childIndex )
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

	void TerrainQuadTree::CalculateLODRanges( std::vector<float>& LODs, float zNear, float zFar ) const
	{
		uint32_t numLODRanges = GetNumLODRanges();
		LODs.resize( numLODRanges );

		const float LODDistanceRatio = 2.0f;

		float total = 0;
		float currentDetailBalance = 0.25f;
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
		
		for ( int32_t mainIndex = 0; mainIndex < static_cast<int32_t>( nodeTransforms.size() ); ++mainIndex )
		{
			Mat4& transform = nodeTransforms[ mainIndex ];
			// order: left - bottom - right - top
			Vec4& tessMult = tessEdgeMult[ mainIndex ];

			Vec2 transformMin, transformMax;
			GetTransformExtentsXZ( transform, transformMin, transformMax );

			//float midX = ( transformMin.x + transformMax.x ) / 2;
			//float midZ = ( transformMin.y + transformMax.y ) / 2;
			//Vec2 top( midX, transformMax.y ), left( transformMin.x, midZ ), bottom( midX, transformMin.y ), right( transformMax.x, midZ );

			for ( const Mat4& otherTransform : nodeTransforms )
			{
				if ( &otherTransform == &transform )
					continue;

				Vec2 otherMin, otherMax;
				GetTransformExtentsXZ( otherTransform, otherMin, otherMax );

				// left
				if ( transformMin.x == otherMax.x && ( transformMax.y >= otherMax.y && transformMin.y <= otherMin.y ) )
				{
					float otherLength = otherMax.y - otherMin.y;
					float thislength = transformMax.y - transformMin.y;
					if ( thislength > otherLength )
					{
						tessMult.x = 2.0f;
					}

					if ( thislength / otherLength == 4.0f )
					{
						transform[ 0 ][ 0 ] *= 0.5f;
						transform[ 3 ][ 0 ] += transform[ 0 ][ 0 ] / 2.0f;

						Vec3 scale( transform[ 0 ][ 0 ], transform[ 1 ][ 1 ], transform[ 2 ][ 2 ] );
						scale.z /= 2.0f;
						Mat4 scaleTransform = glm::scale( scale );

						Vec3 translation( transform[ 3 ] );
						translation.x -= ( transform[ 0 ][ 0 ] );

						Vec3 translation1( translation );
						translation1.z += scale.z / 2.0f;
						Mat4 transform1 = glm::translate( translation1 );
						transform1 *= scaleTransform;

						nodeTransforms.emplace_back( transform1 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						Vec3 translation2( translation );
						translation2.z -= scale.z / 2.0f;
						Mat4 transform2 = glm::translate( translation2 );
						transform2 *= scaleTransform;

						nodeTransforms.emplace_back( transform2 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						break;
					}
				}
				// bottom-edge
				else if ( transformMin.y == otherMax.y && ( transformMax.x >= otherMax.x && transformMin.x <= otherMin.x ) )
				{
					float otherLength = otherMax.x - otherMin.x;
					float thislength = transformMax.x - transformMin.x;
					if ( thislength > otherLength )
					{
						tessMult.y = 2.0f;
					}

					if ( thislength / otherLength == 4.0f )
					{
						transform[ 2 ][ 2 ] *= 0.5f;
						transform[ 3 ][ 2 ] += transform[ 2 ][ 2 ] / 2.0f;

						Vec3 scale( transform[ 0 ][ 0 ], transform[ 1 ][ 1 ], transform[ 2 ][ 2 ] );
						scale.x /= 2.0f;
						Mat4 scaleTransform = glm::scale( scale );

						Vec3 translation( transform[ 3 ] );
						translation.z -= transform[ 2 ][ 2 ];

						Vec3 translation1( translation );
						translation1.x += scale.x / 2.0f;
						Mat4 transform1 = glm::translate( translation1 );
						transform1 *= scaleTransform;

						nodeTransforms.emplace_back( transform1 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						Vec3 translation2( translation );
						translation2.x -= scale.x / 2.0f;
						Mat4 transform2 = glm::translate( translation2 );
						transform2 *= scaleTransform;

						nodeTransforms.emplace_back( transform2 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						break;
					}
				}
				// right-edge
				else if ( transformMax.x == otherMin.x && ( transformMax.y >= otherMax.y && transformMin.y <= otherMin.y ) )
				{
					float otherLength = otherMax.y - otherMin.y;
					float thislength = transformMax.y - transformMin.y;
					if ( thislength > otherLength )
					{
						tessMult.z = 2.0f;
					}

					if ( thislength / otherLength == 4.0f )
					{
						transform[ 0 ][ 0 ] *= 0.5f;
						transform[ 3 ][ 0 ] -= transform[ 0 ][ 0 ] / 2.0f;

						Vec3 scale( transform[ 0 ][ 0 ], transform[ 1 ][ 1 ], transform[ 2 ][ 2 ] );
						scale.z /= 2.0f;
						Mat4 scaleTransform = glm::scale( scale );

						Vec3 translation( transform[ 3 ] );
						translation.x += ( transform[ 0 ][ 0 ] );

						Vec3 translation1( translation );
						translation1.z += scale.z / 2.0f;
						Mat4 transform1 = glm::translate( translation1 );
						transform1 *= scaleTransform;

						nodeTransforms.emplace_back( transform1 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						Vec3 translation2( translation );
						translation2.z -= scale.z / 2.0f;
						Mat4 transform2 = glm::translate( translation2 );
						transform2 *= scaleTransform;

						nodeTransforms.emplace_back( transform2 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						break;
					}
				}
				// top-edge
				else if ( transformMax.y == otherMin.y && ( transformMax.x >= otherMax.x && transformMin.x <= otherMin.x ) )
				{
					float otherLength = otherMax.x - otherMin.x;
					float thislength = transformMax.x - transformMin.x;
					if ( thislength > otherLength )
					{
						tessMult.w = 2.0f;
					}

					if ( thislength / otherLength == 4.0f )
					{
						transform[ 2 ][ 2 ] *= 0.5f;
						transform[ 3 ][ 2 ] -= transform[ 2 ][ 2 ] / 2.0f;

						Vec3 scale( transform[ 0 ][ 0 ], transform[ 1 ][ 1 ], transform[ 2 ][ 2 ] );
						scale.x /= 2.0f;
						Mat4 scaleTransform = glm::scale( scale );

						Vec3 translation( transform[ 3 ] );
						translation.z += transform[ 2 ][ 2 ];

						Vec3 translation1( translation );
						translation1.x += scale.x / 2.0f;
						Mat4 transform1 = glm::translate( translation1 );
						transform1 *= scaleTransform;

						nodeTransforms.emplace_back( transform1 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						Vec3 translation2( translation );
						translation2.x -= scale.x / 2.0f;
						Mat4 transform2 = glm::translate( translation2 );
						transform2 *= scaleTransform;

						nodeTransforms.emplace_back( transform2 );
						tessEdgeMult.emplace_back( Vec4( 1.0f ) );

						break;
					}
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
