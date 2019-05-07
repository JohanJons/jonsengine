#pragma once

#include "Core/Math/AABB.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Frustum.h"
#include "Core/Types.h"
#include "Core/Containers/RangedIterator.hpp"

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
    // "does aabb intersect ..."
    AABBIntersection Intersection(const AABB& aabb, const Mat4& frustumMatrix);
    AABBIntersection Intersection(const AABB& aabb, const Vec3& sphereCentre, const float sphereRadius);
    AABBIntersection Intersection(const AABB& aabb, const FrustumPlanes& frustumPlanes);
    AABBIntersection Intersection(const AABB& aabb, const KDOP& kdop);
    AABBIntersection Intersection(const AABB& aabb, const ConstRangedIterator<KDOP>& rangedKDOP);
    // "does plane intersect ..."
    PlaneIntersection Intersection(const Plane& plane, const AABB& aabb);

    bool IsPointInAABB(const AABB& aabb, const Vec3& point);
    bool IsPointInSphere(const Vec3& sphereCentre, const float radius, const Vec3& point);
    bool IsAABBInKDOP(const KDOP& kdop, const AABB& aabb);
}