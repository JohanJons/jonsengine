#pragma once

#include "include/Core/Math/AABB.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct Plane
    {
        enum class PlaneIntersection
        {
            Front,
            Back,
            Intersect
        };


        Plane(const Vec3& normal, const float distance);
        ~Plane();

        PlaneIntersection Intersection(const AABB& aabb) const;


        Vec3 mNormal;
        float mDistance;
    };
}