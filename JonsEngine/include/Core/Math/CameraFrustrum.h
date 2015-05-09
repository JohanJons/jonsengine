#pragma once

#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <array>


namespace JonsEngine
{
    struct CameraFrustrum
    {
        CameraFrustrum(const Mat4& camViewProjMatrix);
        ~CameraFrustrum();


        std::array<Vec4, 8> mCorners;
        AABB mAABB;
    };
}