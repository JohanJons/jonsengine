#pragma once

#include "include/RenderQueue/RenderableMesh.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
	struct RenderableTerrain
	{
		RenderableTerrain();


		Mat4 mWorldTransform;
		DX11MeshID mMeshID;
		DX11TextureID mHeightMap;
	};
}