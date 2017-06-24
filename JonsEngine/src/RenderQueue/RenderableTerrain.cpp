#include "include/RenderQueue/RenderableTerrain.h"
#include "include/Core/Math/Math.h"

namespace JonsEngine
{
	RenderableTerrain::RenderableTerrain() :
		mWorldTransform(gIdentityMatrix),
		mMeshID(INVALID_DX11_MESH_ID),
		mHeightMap(INVALID_DX11_TEXTURE_ID)
	{
	}
}