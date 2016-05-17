#pragma once

#include "include/RenderQueue/RenderableContainer.h"
#include "include/RenderQueue/RenderableSkeleton.h"
#include "include/RenderQueue/RenderableMaterial.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableMesh : RenderableContainer<RenderableMesh>
    {
        RenderableMesh();


        Mat4 mWorldTransform;
        RenderableSkeleton mSkeleton;
        RenderableMaterial::Index mMaterial;
        float mMaterialTilingFactor;
        DX11MeshID mMeshID;
    };
}