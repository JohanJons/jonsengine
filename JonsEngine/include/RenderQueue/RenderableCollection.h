#pragma once

#include "include/RenderQueue/RenderableMesh.h"

namespace JonsEngine
{
    struct RenderableCollection
    {
        typedef RenderableMesh::Index MeshIndex;

        RenderableCollection();


        MeshIndex mStaticMeshesBegin;
        MeshIndex mStaticMeshesEnd;
        MeshIndex mAnimatedMeshesBegin;
        MeshIndex mAnimatedMeshesEnd;
    };
}