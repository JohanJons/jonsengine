#pragma once

#include "include/RenderQueue/RenderableMesh.h"

namespace JonsEngine
{
    struct RenderableCollection
    {
        RenderableCollection();


		MeshIndex mStaticMeshesBegin;
		MeshIndex mStaticMeshesEnd;
		MeshIndex mAnimatedMeshesBegin;
		MeshIndex mAnimatedMeshesEnd;
    };
}