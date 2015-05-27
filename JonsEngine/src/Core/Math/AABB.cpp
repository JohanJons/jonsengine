#include "include/Core/Math/AABB.h"
#include "include/Core/Math/Math.h"

#include <cmath>

namespace JonsEngine
{
    AABB::AABB(const Vec3& minBounds, const Vec3& maxBounds) : mAABBCenter(0.5f * (minBounds + maxBounds)), mAABBExtent(0.5f * (maxBounds - minBounds))
    {
    }


    Vec3 AABB::Min() const
    {
        return Vec3(mAABBCenter.x - mAABBExtent.x, mAABBCenter.y - mAABBExtent.y, mAABBCenter.z - mAABBExtent.z);
    }

    Vec3 AABB::Max() const
    {
        return Vec3(mAABBCenter.x + mAABBExtent.x, mAABBCenter.y + mAABBExtent.y, mAABBCenter.z + mAABBExtent.z);
    }


    AABB::AABBIntersection AABB::Intersection(const AABB& source) const
	{
		const Vec3 min = source.Min();
		const Vec3 max = source.Max();

		const bool minInAABB = IsPointInAABB(min);
		const bool maxInAABB = IsPointInAABB(max);

		if (minInAABB && maxInAABB)
            return AABBIntersection::Inside;
		else if (minInAABB || maxInAABB)
            return AABBIntersection::Partial;
		else
            return AABBIntersection::Outside;
	}

    AABB::AABBIntersection AABB::Intersection(const Mat4& frustumMatrix) const
    {
        const Vec4 rowX(frustumMatrix[0].x, frustumMatrix[1].x, frustumMatrix[2].x, frustumMatrix[3].x);
        const Vec4 rowY(frustumMatrix[0].y, frustumMatrix[1].y, frustumMatrix[2].y, frustumMatrix[3].y);
        const Vec4 rowZ(frustumMatrix[0].z, frustumMatrix[1].z, frustumMatrix[2].z, frustumMatrix[3].z);
        const Vec4 rowW(frustumMatrix[0].w, frustumMatrix[1].w, frustumMatrix[2].w, frustumMatrix[3].w);

        AABB::AABBIntersection ret(AABBIntersection::Inside);

        // left, right, bottom, top, near, far planes
        std::array<Vec4, 6> planes = { rowW + rowX, rowW - rowX, rowW + rowY, rowW - rowY, rowW + rowZ, rowW - rowZ };
        float d = 0.0f, r = 0.0f;
        for (const Vec4& plane : planes)
        {
            d = glm::dot(Vec3(plane), mAABBCenter);
            r = glm::dot(Vec3(glm::abs(plane)), mAABBExtent);

            if (d - r < -plane.w)
                ret = AABBIntersection::Partial;
            if (d + r < -plane.w)
                return AABBIntersection::Outside;
        }

        return ret;
    }

    AABB::AABBIntersection AABB::Intersection(const Vec3& sphereCentre, const float sphereRadius) const
    {
        float distSquared = std::pow(sphereRadius, 2.0f);
        const Vec3 min = Min();
        const Vec3 max = Max();

		if (IsPointInSphere(min, sphereCentre, sphereRadius) && IsPointInSphere(max, sphereCentre, sphereRadius))
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

    bool AABB::IsPointInAABB(const Vec3& point) const
    {
        const Vec3 min = Min();
        const Vec3 max = Max();

        return min.x <= point.x && point.x <= max.x &&
               min.y <= point.y && point.y <= max.y &&
               min.z <= point.z && point.y <= max.z;
    }


    AABB operator*(const Mat4& transform, const AABB& aabb)
    {
        const Vec3 oldMin = aabb.Min();
        const Vec3 oldMax = aabb.Max();

        float av, bv;
        Vec3 min(transform[3].x, transform[3].y, transform[3].z);
        Vec3 max(transform[3].x, transform[3].y, transform[3].z);
        for (uint32_t i = 0; i < 3; i++)
        {
            for (uint32_t j = 0; j < 3; j++)
            {
                av = transform[i][j] * oldMin[j];
                bv = transform[i][j] * oldMax[j];

                if (av < bv)
                {
                    min[i] += av;
                    max[i] += bv;
                }
                else {
                    min[i] += bv;
                    max[i] += av;
                }
            }
        }

        return AABB(min, max);
    }

    AABB operator*(const AABB& aabb, const Mat4& transform)
    {
        return transform * aabb;
    }
}