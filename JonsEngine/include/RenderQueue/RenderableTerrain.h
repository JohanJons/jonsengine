#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
	struct RenderableTerrain : RenderableContainer<RenderableTerrain>
	{
		RenderableTerrain();


		DX11MeshID mMeshID;
		DX11TextureID mHeightMap;
	};

	typedef RenderableTerrain::ContainerType RenderableTerrainContainer;
}