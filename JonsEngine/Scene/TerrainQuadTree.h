#pragma once

#include "Core/Types.h"
#include "Core/Math/AABB.h"
#include "RenderQueue/RenderQueueTypes.h"

#include <vector>
#include <deque>
#include <bitset>

namespace JonsEngine
{
	struct QuadNodeAABB
	{
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

	enum class QuadNodeCullStatus
	{
		Undefined,
		OutOfFrustum,
		OutOfRange,
		Added
	};

	class TerrainQuadTree
	{
	public:
		TerrainQuadTree() = default;
		TerrainQuadTree( const std::vector<uint8_t>& heightmapData, uint32_t width, uint32_t height, uint32_t patchMinSize, float heightmapScale, const Mat4& worldTransform );

		void CullNodes( std::vector<Mat4>& renderableTransforms, std::vector<float>& LODRanges, std::vector<Vec2>& morphConstants, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform,
			float zNear, float zFar, uint32_t maxPatchSize ) const;

		uint32_t GetNumNodes() const { return static_cast<uint32_t>( mGridTraversal.size() ); }
		uint32_t GetPatchMinSize() const { return mPatchMinSize; }
		float GetHeightmapScale() const { return mHeightmapScale; }
		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		void CreateGridNode( QuadNodeAABB* parent, float centerX, float centerZ, float width, float height, uint32_t LODlevel, float yTranslation );
		// local space during function
		void ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, uint32_t heightmapWidth, uint32_t LODlevel, float yTranslation );

		uint32_t ExpectedNumNodes( float width, uint32_t patchMinSize ) const;
		bool ValidateCulledNodes( std::vector<Mat4>& renderableTransforms ) const;

		void AddNode( std::vector<Mat4>& renderableTransforms, const QuadNodeAABB& quadAABB, uint32_t maxPatchSize, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum ) const;
		void AddNode( std::vector<Mat4>& renderableTransforms, const QuadNodeAABB& quadAABB, uint32_t maxPatchSize, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum, bool addBL, bool addBR, bool addTR, bool addTL ) const;
		QuadNodeCullStatus TerrainQuadTree::CullQuad( std::vector<Mat4>& renderableTransforms, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform,
			const std::vector<float>& LODRanges, bool parentFullyInFrustum, bool parentTooLargePatch, uint32_t maxPatchSize ) const;
		void CalculateLODRanges( std::vector<float>& LODs, std::vector<Vec2>& morphConstants, float zNear, float zFar, uint32_t maxPatchSize ) const;

		uint32_t mPatchMinSize;
		float mHeightmapScale;
		std::vector<QuadNodeAABB> mGridTraversal;
	};
}
