#pragma once

#include "include/RenderQueue/RenderableMesh.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderableCamera
    {
        typedef RenderableMesh::Index MeshIndex;


        Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
        Mat4 mCameraViewProjectionMatrix;
        Vec3 mCameraPosition;
        float mFOV;

        MeshIndex mStaticMeshesBegin;
        MeshIndex mStaticMeshesEnd;
        MeshIndex mAnimatedMeshesBegin;
        MeshIndex mAnimatedMeshesEnd;
    };
}