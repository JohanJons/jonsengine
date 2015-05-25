#include "include/Core/Math/Plane.h"

#include "include/Core/Math/AABB.h"

namespace JonsEngine
{
    Plane::Plane()
    {
    }

    Plane::Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3) : mNormal(glm::normalize(glm::cross(point2 - point1, point3 - point2))), mDistance(glm::dot(mNormal, point1))
    {
    }

    Plane::~Plane()
    {
    }


    Plane::PlaneIntersection Plane::Intersection(const AABB& aabb) const
    {
        const float radius = aabb.mAABBExtent.x * glm::abs(mNormal.x) +
                             aabb.mAABBExtent.y * glm::abs(mNormal.y) +
                             aabb.mAABBExtent.z * glm::abs(mNormal.z);

        const float distance = glm::dot(mNormal, aabb.mAABBCenter) - mDistance;

        if (glm::abs(distance) <= radius)
            return PlaneIntersection::Intersect;

        return distance > radius ? PlaneIntersection::Front : PlaneIntersection::Back;
    }
}