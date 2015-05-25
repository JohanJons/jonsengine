#pragma once

#include "include/Core/Types.h"

namespace JonsEngine
{
    struct AABB;

    struct Plane
    {
        enum class PlaneIntersection
        {
            Front,
            Back,
            Intersect
        };


        Plane();
        Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3);
        ~Plane();

        PlaneIntersection Intersection(const AABB& aabb) const;


        Vec3 mNormal;
        float mDistance;
    };
}