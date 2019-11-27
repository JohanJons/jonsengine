#pragma once

#include "Core/Types.h"
#include "Core/Math/AABB.h"

#include <vector>
#include <deque>
#include "boost/dynamic_bitset.hpp"

namespace JonsEngine
{
	struct QuadNodeAABB
	{
		enum ChildNode : int32_t
		{
			INVALID = -1,
			TOP_LEFT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT,
			TOP_RIGHT,
			NUM_CHILDREN
		};
		static_assert( ChildNode::TOP_LEFT == 0, "Be mindful of offsets" );

		QuadNodeAABB( const Vec3& frustumMin, const Vec3& frustumMax, QuadNodeAABB* parent, uint32_t LODIndex ) :
			mFrustumAABB( frustumMin, frustumMax ),
			mParent( parent ),
			mLODIndex( LODIndex )
		{ }

		// world space
		AABB mFrustumAABB;
		uint32_t mLODIndex = 0;
		QuadNodeAABB* mParent = nullptr;
		QuadNodeAABB* mChildBegin = nullptr;
	};

	struct QuadNodeNeighbours
	{
		enum Facing : uint32_t
		{
			LEFT = 0,
			BOTTOM,
			RIGHT,
			TOP,
			NUM_OFFSETS
		};

		std::array<const QuadNodeAABB*, 4> mSameLODNeighbours;
	};

	enum class QuadNodeCullStatus
	{
		OutOfFrustum,
		OutOfRange,
		Added
	};

	struct PerCullData
	{
		PerCullData( uint32_t numTreeNodes );

		std::vector<const QuadNodeAABB*> mHighestLODNodes;
		std::vector<QuadNodeCullStatus> mIntersectionResults;
		std::deque<const QuadNodeAABB*> mNodeQueue;
		boost::dynamic_bitset<> mNodeAddedLookup;
	};

	class TerrainQuadTree
	{
	public:
		TerrainQuadTree() = default;
		TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform );

		void CullNodes( std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult, std::vector<float>& LODRanges, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const;

		uint32_t GetNumLODRanges() const;
		uint32_t GetNumNodes() const { return static_cast<uint32_t>( mGridTraversal.size() ); }
		uint32_t GetPatchMinSize() const { return mPatchMinSize; }
		float GetHeightmapScale() const { return mHeightmapScale; }
		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		void CreateGridNode( QuadNodeAABB* parent, float centerX, float centerZ, float width, float height, uint32_t LODlevel, float yTranslation );
		// local space during function
		void ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth, uint32_t LODlevel, float yTranslation );
		void ComputeNeighbours();
		bool ValidateNeighbours() const;

		uint32_t ExpectedNumNodes( float width, uint32_t patchMinSize ) const;



		void AddNode2( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const std::vector<float>& LODRanges ) const;
		QuadNodeCullStatus TerrainQuadTree::CullQuad( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const;



		void CalculateLODRanges( std::vector<float>& LODs, float zNear, float zFar ) const;
		QuadNodeCullStatus CalculateHighestLODNodes( PerCullData& cullData, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const;
		void CalculateNodeTransforms( PerCullData& cullData, std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult, const std::vector<float>& LODRanges ) const;
		void CalculateTessellationFactors( std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult ) const;

		uint32_t mPatchMinSize;
		float mHeightmapScale;
		std::vector<QuadNodeAABB> mGridTraversal;
		std::vector<QuadNodeNeighbours> mGridNeighbours;
	};
}
