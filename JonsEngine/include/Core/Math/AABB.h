#pragma once

#include "include/Core/Math/KDOP.h"
#include "include/Core/Types.h"

#include <vector>


namespace JonsEngine
{
    struct AABB
    {
        AABB(const Vec3& minBounds, const Vec3& maxBounds);

        Vec3 Min() const;
        Vec3 Max() const;


        const Vec3 mAABBCenter;
        const Vec3 mAABBExtent;
    };

    AABB operator*(const Mat4& transform, const AABB& aabb);
    AABB operator*(const AABB& aabb, const Mat4& transform);
}