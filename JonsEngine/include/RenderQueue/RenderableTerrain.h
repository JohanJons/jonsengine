#pragma once

#include "include/RenderQueue/RenderableMesh.h"
#include "include/RenderQueue/AABBRenderData.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
	struct RenderableTerrainData
	{
		RenderableTerrainData(DX11TextureID heightMap, std::size_t endIndex, float heightScale, float patchSize) :
			mHeightMap(heightMap),
			mEndIndex(endIndex),
			mHeightScale(heightScale),
			mPatchSize(patchSize)
		{ }

		DX11TextureID mHeightMap;
		std::size_t mEndIndex;
		float mHeightScale;
		float mPatchSize;
	};

	struct RenderableTerrains
	{
		std::vector<RenderableTerrainData> mTerrainData;
		std::vector<Mat4> mTransforms;
		RenderableAABBsContainer mDebugAABBs;

		uint32_t GetNumTerrains() const { return mTerrainData.size(); }
	};
}