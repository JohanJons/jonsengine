#pragma once

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


        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };

    AABB operator*(const Mat4& transform, const AABB& aabb);
    AABB operator*(const AABB& aabb, const Mat4& transform);

    enum class AABBIntersection
    {
        AABB_INTERSECTION_INSIDE,
        AABB_INTERSECTION_PARTIAL,
        AABB_INTERSECTION_OUTSIDE
    };


    // note: make sure frustrum and aabb are in the same space
    AABBIntersection IsAABBInFrustum(const AABB& aabb, const Mat4& frustumMatrix);
    AABBIntersection IsAABBInSphere(const AABB& aabb, const Vec3& sphereCentre, const float sphereRadius);
    
    // "is source in target"
    AABBIntersection IsAABBInAABB(const AABB& source, const AABB& target);
	bool IsPointInSphere(const Vec3& point, const Vec3& sphereCentre, const float radius);
    bool IsPointInAABB(const Vec3& point, const AABB& aabb);
}