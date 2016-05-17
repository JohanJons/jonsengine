#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderableCamera : RenderableCollection
    {
        RenderableCamera();


        Mat4 mCameraViewMatrix;
        Mat4 mCameraProjectionMatrix;
        Mat4 mCameraViewProjectionMatrix;
        Vec3 mCameraPosition;
        float mFOV;
    };
}