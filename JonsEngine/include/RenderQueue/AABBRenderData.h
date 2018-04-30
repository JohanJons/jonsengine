#pragma once

#include "include/RenderQueue/RenderableCamera.h"
#include "include/Core/Types.h"

#include <vector>
#include <utility>

namespace JonsEngine
{
	typedef std::pair<Mat4, DX11MeshID> RenderAABBPair;
	typedef std::vector<RenderAABBPair> RenderableAABBsContainer;
}
