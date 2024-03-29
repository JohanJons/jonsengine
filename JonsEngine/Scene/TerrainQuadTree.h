#pragma once

#include "Core/Types.h"
#include "Core/Math/AABB.h"
#include "RenderQueue/RenderQueueTypes.h"

#include <vector>
#include <deque>
#include <bitset>

namespace JonsEngine
{
	struct QuadTreeCullData
	{
		std::vector<Mat4>& mTransforms;
		std::vector<uint32_t>& mLODs;
		std::vector<float>& mLODRanges;
		std::vector<Vec2>& mMorphConstants;
	};

	struct QuadNodeAABB
	{
		QuadNodeAABB( const Vec3& frustumMin, const Vec3& frustumMax, QuadNodeAABB* parent, uint32_t level ) :
			mFrustumAABB( frustumMin, frustumMax ),
			mParent( parent ),
			mTreeLevel( level )
		{ }

		// world space
		AABB mFrustumAABB;
		uint32_t mTreeLevel = 0;
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
		TerrainQuadTree( const std::vector<uint8_t>& heightmapData, TextureType heightmapType, uint32_t width, uint32_t height, uint32_t patchMinSize, uint32_t patchMaxSize, float heightmapScale, const Mat4& worldTransform );

		void CullNodes( QuadTreeCullData& outData, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, float zNear, float zFar ) const;

		uint32_t GetNumNodes() const { return static_cast<uint32_t>( mGridTraversal.size() ); }
		uint32_t GetPatchMinSize() const { return mPatchMinSize; }
		uint32_t GetPatchMaxSize() const { return mPatchMaxSize; }
		uint32_t GetNumLODLevels() const { return mNumLODs; }
		float GetHeightmapScale() const { return mHeightmapScale; }
		void GetWorldXZBounds( Vec2& worldMin, Vec2& worldMax ) const;

	private:
		void CreateGridNode( QuadNodeAABB* parent, float centerX, float centerZ, float width, float height, uint32_t level );
		// local space during function
		void ProcessQuadNode( QuadNodeAABB& quadAABB, const std::vector<uint8_t>& heightmapData, TextureType heightmapType, uint32_t heightmapWidth, uint32_t heightmapHeight, uint32_t treeLevel );

		uint32_t ExpectedNumNodes( float width, uint32_t patchMinSize ) const;
		bool ValidateCulledNodes( std::vector<Mat4>& renderableTransforms ) const;

		void AddNode( QuadTreeCullData& outData, const QuadNodeAABB& quadAABB, uint32_t LODLevel, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum ) const;
		void AddNode( QuadTreeCullData& outData, const QuadNodeAABB& quadAABB, uint32_t LODLevel, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum, bool addBL, bool addBR, bool addTR, bool addTL ) const;
		QuadNodeCullStatus TerrainQuadTree::CullQuad( QuadTreeCullData& outData, const QuadNodeAABB& quadAABB, const Vec3& cameraWorldPos, const Mat4& cameraViewProjTransform, bool parentFullyInFrustum, bool parentTooLargePatch ) const;

		uint32_t GetPatchSize( const QuadNodeAABB& quadAABB ) const;
		uint32_t GetLODLevel( const QuadNodeAABB& quadAABB ) const;
		void CalculateLODRanges( QuadTreeCullData& outData, float zNear, float zFar ) const;
		uint32_t CalculateNumLODs( uint32_t width, uint32_t height ) const;

		uint32_t mPatchMinSize = 0;
		uint32_t mPatchMaxSize = 0;
		uint32_t mNumLODs = 0;
		float mHeightmapScale;
		float mWorldYTranslation;
		std::vector<QuadNodeAABB> mGridTraversal;
	};
}
