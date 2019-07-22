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

		QuadNodeAABB( const Vec3& min, const Vec3& max ) :
			mAABB( min, max )
		{ }

		// world space
		AABB mAABB;
		QuadNodeAABB* mChildBegin = nullptr;
	};

	class TerrainQuadTree
	{
	public:
		TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform );

		void CullNodes( std::vector<Mat4>& nodeTransforms, const Mat4& cameraViewProjTransform ) const;

		uint32_t GetNumLODRanges() const;
		uint32_t GetNumNodes() const { return static_cast<uint32_t>( mGridTraversal.size() ); }
		uint32_t GetPatchMinSize() const { return mPatchMinSize; }
		float GetHeightmapScale() const { return mHeightmapScale; }
		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		void CullQuad( std::vector<Mat4>& nodes, const QuadNodeAABB& quadAABB, const Mat4& cameraViewProjTransform ) const;
		uint32_t ExpectedNumNodes( uint32_t width, uint32_t patchMinSize ) const;
		void AddGridNode( uint32_t centerX, uint32_t centerZ, uint32_t width, uint32_t height );
		// local space during function
		void ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth );

		uint32_t mPatchMinSize;
		float mHeightmapScale;
		std::vector<QuadNodeAABB> mGridTraversal;
	};
}
