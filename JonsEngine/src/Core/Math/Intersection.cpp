#include "include/Core/Math/Intersection.h"

namespace JonsEngine
{
    AABBIntersection Intersection(const AABB& target, const AABB& source)
    {
        const Vec3 min = target.Min();
        const Vec3 max = target.Max();

        const bool minInAABB = IsPointInAABB(source, min);
        const bool maxInAABB = IsPointInAABB(source, max);

        if (minInAABB && maxInAABB)
            return AABBIntersection::Inside;
        else if (minInAABB || maxInAABB)
            return AABBIntersection::Partial;
        else
            return AABBIntersection::Outside;
    }

    AABBIntersection Intersection(const AABB& aabb, const Mat4& frustumMatrix)
    {
        const Vec4 rowX(frustumMatrix[0].x, frustumMatrix[1].x, frustumMatrix[2].x, frustumMatrix[3].x);
        const Vec4 rowY(frustumMatrix[0].y, frustumMatrix[1].y, frustumMatrix[2].y, frustumMatrix[3].y);
        const Vec4 rowZ(frustumMatrix[0].z, frustumMatrix[1].z, frustumMatrix[2].z, frustumMatrix[3].z);
        const Vec4 rowW(frustumMatrix[0].w, frustumMatrix[1].w, frustumMatrix[2].w, frustumMatrix[3].w);

        AABBIntersection ret(AABBIntersection::Inside);

        // left, right, bottom, top, near, far planes
        std::array<Vec4, 6> planes = { rowW + rowX, rowW - rowX, rowW + rowY, rowW - rowY, rowW + rowZ, rowW - rowZ };
        float d = 0.0f, r = 0.0f;
        for (const Vec4& plane : planes)
        {
            d = glm::dot(Vec3(plane), aabb.mAABBCenter);
            r = glm::dot(Vec3(glm::abs(plane)), aabb.mAABBExtent);

            if (d - r < -plane.w)
                ret = AABBIntersection::Partial;
            if (d + r < -plane.w)
                return AABBIntersection::Outside;
        }

        return ret;
    }

    AABBIntersection Intersection(const AABB& aabb, const Vec3& sphereCentre, const float sphereRadius)
    {
        float distSquared = std::pow(sphereRadius, 2.0f);
        const Vec3 min = aabb.Min();
        const Vec3 max = aabb.Max();

        if (IsPointInSphere(sphereCentre, sphereRadius, min) && IsPointInSphere(sphereCentre, sphereRadius, max))
            return AABBIntersection::Inside;

        if (sphereCentre.x < min.x)
            distSquared -= std::pow(sphereCentre.x - min.x, 2.0f);
        else if (sphereCentre.x > max.x)
            distSquared -= std::pow(sphereCentre.x - max.x, 2.0f);
        if (sphereCentre.y < min.y)
            distSquared -= std::pow(sphereCentre.y - min.y, 2.0f);
        else if (sphereCentre.y > max.y)
            distSquared -= std::pow(sphereCentre.y - max.y, 2.0f);
        if (sphereCentre.z < min.z)
            distSquared -= std::pow(sphereCentre.z - min.z, 2.0f);
        else if (sphereCentre.z > max.z)
            distSquared -= std::pow(sphereCentre.z - max.z, 2.0f);

        return distSquared > 0 ? AABBIntersection::Partial : AABBIntersection::Outside;
    }


    PlaneIntersection Intersection(const Plane& plane, const AABB& aabb)
    {
        const float radius = aabb.mAABBExtent.x * glm::abs(plane.mNormal.x) +
                             aabb.mAABBExtent.y * glm::abs(plane.mNormal.y) +
                             aabb.mAABBExtent.z * glm::abs(plane.mNormal.z);

        const float distance = glm::dot(plane.mNormal, aabb.mAABBCenter) - plane.mDistance;

        if (glm::abs(distance) <= radius)
            return PlaneIntersection::Intersect;

        return distance > radius ? PlaneIntersection::Front : PlaneIntersection::Back;
    }


    bool IsPointInAABB(const AABB& aabb, const Vec3& point)
    {
        const Vec3 min = aabb.Min();
        const Vec3 max = aabb.Max();

        return min.x <= point.x && point.x <= max.x &&
               min.y <= point.y && point.y <= max.y &&
               min.z <= point.z && point.y <= max.z;
    }

    bool IsPointInSphere(const Vec3& sphereCentre, const float radius, const Vec3& point)
    {
        return glm::distance(sphereCentre, point) <= radius;
    }
}