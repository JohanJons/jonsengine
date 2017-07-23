#include "include/RenderQueue/RenderableTerrain.h"
#include "include/Core/Math/Math.h"

namespace JonsEngine
{
	RenderableTerrain::RenderableTerrain() :
		mWorldTransform(gIdentityMatrix),
		mHeightMap(INVALID_DX11_TEXTURE_ID)
	{
	}

	RenderableTerrain::RenderableTerrain(const Mat4& transform, DX11TextureID heightmap) :
		mWorldTransform(transform),
		mHeightMap(heightmap)
	{
	}
}