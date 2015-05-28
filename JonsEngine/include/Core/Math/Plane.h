#pragma once

#include "include/Core/Types.h"

#include <array>
#include <vector>

namespace JonsEngine
{
    struct Plane
    {
        Plane(const Vec4& planeCoefficients);
        Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3);
        ~Plane();


        const Vec3 mNormal;
        const float mDistance;
    };

    typedef std::array<Plane, 6> FrustumPlanes;
    typedef std::vector<Plane> KDOP;
}