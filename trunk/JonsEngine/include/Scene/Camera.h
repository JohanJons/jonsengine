#pragma once

#include "include/Core/Types.h"

namespace JonsEngine
{
    /* Camera definition */
    struct Camera
    {
        Vec3 mCameraPosition;
        Vec3 mTargetVector;
        Vec3 mUpVector;

        Camera();
    };


    /* Camera inlines */
    inline Camera::Camera() : mCameraPosition(0.0f), mTargetVector(0.0f, 0.0f, -1.0f), mUpVector(0.0f, 1.0f, 0.0f)
    {
    }
}