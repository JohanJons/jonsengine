#include "include/RenderQueue/RenderableTerrain.h"
#include "include/Core/Math/Math.h"

namespace JonsEngine
{
	RenderableTerrain::RenderableTerrain() :
		mWorldTransform(gIdentityMatrix),
		mHeightMap(INVALID_DX11_TEXTURE_ID),
		mPatchSize(0)
	{
	}

	RenderableTerrain::RenderableTerrain(const Mat4& transform, const DX11TextureID heightmap, const float heightScale, const float patchSize) :
		mWorldTransform(transform),
		mHeightMap(heightmap),
		mHeightScale(heightScale),
		mPatchSize(patchSize)
	{
	}
}