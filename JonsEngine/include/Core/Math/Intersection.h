#pragma once

#include "include/Core/Math/AABB.h"
#include "include/Core/Math/Plane.h"
#include "include/Core/Math/KDOP.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    enum class AABBIntersection
    {
        Inside,
        Partial,
        Outside
    };

    enum class PlaneIntersection
    {
        Front,
        Back,
        Intersect
    };


    // "is target in source"
    AABBIntersection Intersection(const AABB& target, const AABB& source);
    AABBIntersection Intersection(const AABB& aabb, const Mat4& frustumMatrix);
    AABBIntersection Intersection(const AABB& aabb, const Vec3& sphereCentre, const float sphereRadius);
    template <uint32_t MAX_KDOP_PLANES>
    AABBIntersection Intersection(const AABB& aabb, const KDOP<MAX_KDOP_PLANES>& kdop);

    PlaneIntersection Intersection(const Plane& plane, const AABB& aabb);

    bool IsPointInAABB(const AABB& aabb, const Vec3& point);
    bool IsPointInSphere(const Vec3& sphereCentre, const float radius, const Vec3& point);


    template <uint32_t MAX_KDOP_PLANES>
    inline AABBIntersection Intersection(const AABB& aabb, const KDOP<MAX_KDOP_PLANES>& kdop)
    {
        auto ret = AABBIntersection::Inside;

        for (uint32_t index = 0; index < kdop.TotalPlanes(); ++index)
        {
            const Plane& plane = kdop[index];

            const auto planeIntersection = Intersection(aabb);
            // if the AABB is behind any of the planes, it is not inside the k-dop
            if (planeIntersection == PlaneIntersection::Back)
                return AABBIntersection::Outside;
            if (planeIntersection == PlaneIntersection::Intersect)
                ret = AABBIntersection::Partial;
        }

        return ret;
    }
}