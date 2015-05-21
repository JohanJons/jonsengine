#pragma once

#include "include/Core/Math/KDOP.h"
#include "include/Core/Types.h"

#include <vector>


namespace JonsEngine
{
    struct AABB
    {
        enum class AABBIntersection
        {
            Inside,
            Partial,
            Outside
        };


        AABB(const Vec3& center, const Vec3& extent);
        AABB(const CameraFrustrum& cameraFrustrum);

		AABB& operator*=(const Mat4& transform);

        Vec3 Min() const;
        Vec3 Max() const;

        // "is source in this"
        AABBIntersection Intersection(const AABB& source) const;
        AABBIntersection Intersection(const Mat4& frustumMatrix) const;
        AABBIntersection Intersection(const Vec3& sphereCentre, const float sphereRadius) const;
        template <uint32_t MAX_KDOP_PLANES>
        AABBIntersection Intersection(const KDOP<MAX_KDOP_PLANES>& kdop) const;
        bool IsPointInAABB(const Vec3& point) const;


        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };

    AABB operator*(const Mat4& transform, const AABB& aabb);
    AABB operator*(const AABB& aabb, const Mat4& transform);


    template <uint32_t MAX_KDOP_PLANES>
    inline AABB::AABBIntersection AABB::Intersection(const KDOP<MAX_KDOP_PLANES>& kdop) const
    {
        auto ret = AABBIntersection::Inside;

        for (const Plane& plane : kdop)
        {
            auto planeIntersection = plane.Intersection(*this);
            // if the AABB is behind any of the planes, it is not inside the k-dop
            if (planeIntersection == Plane::PlaneIntersection::Back)
                return AABBIntersection::Outside;
            if (planeIntersection == Plane::PlaneIntersection::Intersect)
                ret = AABBIntersection::Partial;
        }

        return ret;
    }
}