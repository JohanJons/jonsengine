#pragma once

#include "Core/Types.h"
#include "Core/Math/AABB.h"

#include <vector>

namespace JonsEngine
{
	struct QuadNodeAABB
	{
		enum ChildOffset : uint32_t
		{
			TOP_LEFT = 0,
			TOP_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT,
			NUM_CHILDREN
		};

		QuadNodeAABB( const Vec3& min, const Vec3& max, uint32_t LODIndex ) :
			mAABB( min, max ),
			mLODIndex( LODIndex )
		{ }

		// world space
		AABB mAABB;
		uint32_t mLODIndex = 0;
		QuadNodeAABB* mChildBegin = nullptr;
	};

	class TerrainQuadTree
	{
	public:
		TerrainQuadTree() { }
		TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform );

		void CullNodes( std::vector<Mat4>& nodeTransforms, std::vector<Vec4>& tessEdgeMult, std::vector<float>& LODRanges, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const;

		uint32_t GetNumLODRanges() const;
		uint32_t GetNumNodes() const { return static_cast<uint32_t>( mGridTraversal.size() ); }
		uint32_t GetPatchMinSize() const { return mPatchMinSize; }
		float GetHeightmapScale() const { return mHeightmapScale; }
		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		void AddNode( std::vector<Mat4>& nodes, std::vector<Vec4>& tessEdgeMult, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const std::vector<float>& LODRanges ) const;
		bool CullQuad( std::vector<Mat4>& nodes, std::vector<Vec4>& tessEdgeMult, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, const std::vector<float>& LODRanges, bool parentFullyInFrustum ) const;
		uint32_t ExpectedNumNodes( uint32_t width, uint32_t patchMinSize ) const;
		void CreateGridNode( uint32_t centerX, uint32_t centerZ, uint32_t width, uint32_t height, uint32_t LODlevel );
		// local space during function
		void ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth, uint32_t LODlevel );
		void CalculateLODRanges( std::vector<float>& LODs, float zNear, float zFar ) const;

		uint32_t mPatchMinSize;
		float mHeightmapScale;
		std::vector<QuadNodeAABB> mGridTraversal;
	};
}
