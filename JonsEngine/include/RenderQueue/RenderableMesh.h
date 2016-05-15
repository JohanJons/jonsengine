#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh
    {
        Mat4 mWorldTransform;
        RenderableSkeleton mSkeleton;
        DX11MeshID mMeshID;
    };

    typedef std::vector<RenderableMesh> RenderableMeshes;
}