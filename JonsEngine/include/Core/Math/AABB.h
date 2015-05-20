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
            AABB_INTERSECTION_INSIDE,
            AABB_INTERSECTION_PARTIAL,
            AABB_INTERSECTION_OUTSIDE
        };


        AABB(const Vec3& center, const Vec3& extent);
        AABB(const CameraFrustrum& cameraFrustrum);

		AABB& operator*=(const Mat4& transform);

        Vec3 Min() const;
        Vec3 Max() const;

        // "is source in this"
        AABBIntersection IsAABBInAABB(const AABB& source) const;
        AABBIntersection IsAABBInFrustum(const Mat4& frustumMatrix) const;
        AABBIntersection IsAABBInSphere(const Vec3& sphereCentre, const float sphereRadius) const;

        template <uint32_t MAX_KDOP_PLANES>
        AABBIntersection IsAABBInKDOP(const KDOP<MAX_KDOP_PLANES>& kdop) const
        {

        }

        bool IsPointInAABB(const Vec3& point) const;


        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };


    AABB operator*(const Mat4& transform, const AABB& aabb);
    AABB operator*(const AABB& aabb, const Mat4& transform);
}