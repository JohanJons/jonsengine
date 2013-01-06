#pragma once

#include "include/Core/Types.h"

namespace JonsEngine
{
    /* Camera definition */
    struct Camera
    {
        Vec3 CameraPosition;
        Vec3 TargetVector;
        Vec3 UpVector;

        Camera();
    };


    /* Camera inlines */
    inline Camera::Camera() : CameraPosition(0.0f), TargetVector(0.0f, 0.0f, -1.0f), UpVector(0.0f, 1.0f, 0.0f)
    {
    }
}