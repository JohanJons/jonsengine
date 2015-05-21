#include "include/Core/Math/Plane.h"

namespace JonsEngine
{
    Plane::Plane(const Vec3& normal, const float distance) : mNormal(normal), mDistance(distance)
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