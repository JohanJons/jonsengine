#pragma once

#include "include/RenderQueue/RenderableMesh.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
	struct RenderableTerrainData
	{
		DX11TextureID mHeightMap;
		std::size_t mEndIndex;
		float mHeightScale;
		float mPatchSize;
	};

	struct RenderableTerrains
	{
		std::vector<RenderableTerrainData> mTerrainData;
		std::vector<Mat4> mTransforms;
	};
}