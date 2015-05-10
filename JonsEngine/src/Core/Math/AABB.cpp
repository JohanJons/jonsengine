#include "include/Core/Math/AABB.h"
#include "include/Core/Math/Math.h"

#include <cmath>

namespace JonsEngine
{
	AABB::AABB(const Vec3& center, const Vec3& extent) : mAABBCenter(center), mAABBExtent(extent)
    {
    }

    AABB::AABB(const CameraFrustrum& cameraFrustrum)
    {
        Vec4 minBounds(std::numeric_limits<float>::max()), maxBounds(-std::numeric_limits<float>::max());

        for (const Vec4& corner : cameraFrustrum)
        {
            minBounds = MinVal(minBounds, corner);
            maxBounds = MaxVal(maxBounds, corner);
        }

        mAABBCenter = Vec3(0.5f * (minBounds + maxBounds));
        mAABBExtent = Vec3(0.5f * (maxBounds - minBounds));
    }


    AABB& AABB::operator*=(const Mat4& transform)
	{
        // TODO: perhaps this can be simplified
        const Vec4 transformedMin = transform * Vec4(Min(), 1.0f);
        const Vec4 transformedMax = transform * Vec4(Max(), 1.0f);

        mAABBCenter.x = 0.5f * (transformedMin.x + transformedMax.x);
        mAABBCenter.y = 0.5f * (transformedMin.y + transformedMax.y);
        mAABBCenter.z = 0.5f * (transformedMin.z + transformedMax.z);

        mAABBExtent.x = 0.5f * (transformedMax.x - transformedMin.x);
        mAABBExtent.y = 0.5f * (transformedMax.y - transformedMin.y);
        mAABBExtent.z = 0.5f * (transformedMax.z - transformedMin.z);

        return *this;
	}


    Vec3 AABB::Min() const
    {
        return Vec3(mAABBCenter.x - mAABBExtent.x, mAABBCenter.y - mAABBExtent.y, mAABBCenter.z - mAABBExtent.z);
    }

    Vec3 AABB::Max() const
    {
        return Vec3(mAABBCenter.x + mAABBExtent.x, mAABBCenter.y + mAABBExtent.y, mAABBCenter.z + mAABBExtent.z);
    }

    AABB operator*(const Mat4& transform, const AABB& aabb)
	{
        AABB ret(aabb);

        ret *= transform;

        return ret;
	}

    AABB operator*(const AABB& aabb, const Mat4& transform)
	{
        AABB ret(aabb);

        ret *= transform;

        return ret;
	}


    AABBIntersection IsAABBInFrustum(const AABB& aabb, const Mat4& frustumMatrix)
    {
        const Vec4 rowX(frustumMatrix[0].x, frustumMatrix[1].x, frustumMatrix[2].x, frustumMatrix[3].x);
        const Vec4 rowY(frustumMatrix[0].y, frustumMatrix[1].y, frustumMatrix[2].y, frustumMatrix[3].y);
        const Vec4 rowZ(frustumMatrix[0].z, frustumMatrix[1].z, frustumMatrix[2].z, frustumMatrix[3].z);
        const Vec4 rowW(frustumMatrix[0].w, frustumMatrix[1].w, frustumMatrix[2].w, frustumMatrix[3].w);

        AABBIntersection ret(AABBIntersection::AABB_INTERSECTION_INSIDE);

        // left, right, bottom, top, near, far planes
        std::array<Vec4, 6> planes = { rowW + rowX, rowW - rowX, rowW + rowY, rowW - rowY, rowW + rowZ, rowW - rowZ };
        float d = 0.0f, r = 0.0f;
        for (const Vec4& plane : planes)
        {
            d = glm::dot(Vec3(plane), aabb.mAABBCenter);
            r = glm::dot(Vec3(glm::abs(plane)), aabb.mAABBExtent);

            if (d - r < -plane.w)
                ret = AABBIntersection::AABB_INTERSECTION_PARTIAL;
            if (d + r < -plane.w)
                return AABBIntersection::AABB_INTERSECTION_OUTSIDE;
        }

        return ret;
    }

    AABBIntersection IsAABBInSphere(const AABB& aabb, const Vec3& sphereCentre, const float sphereRadius)
    {
        float distSquared = std::pow(sphereRadius, 2.0f);
        const Vec3 min = aabb.Min();
        const Vec3 max = aabb.Max();

		if (IsPointInSphere(min, sphereCentre, sphereRadius) && IsPointInSphere(max, sphereCentre, sphereRadius))
			return AABBIntersection::AABB_INTERSECTION_INSIDE;

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

        return distSquared > 0 ? AABBIntersection::AABB_INTERSECTION_PARTIAL : AABBIntersection::AABB_INTERSECTION_OUTSIDE;
    }

    AABBIntersection IsAABBInAABB(const AABB& source, const AABB& target)
    {
        const Vec3 min = source.Min();
        const Vec3 max = source.Max();

        const bool minInAABB = IsPointInAABB(min, target);
        const bool maxInAABB = IsPointInAABB(max, target);

        if (minInAABB && maxInAABB)
            return AABBIntersection::AABB_INTERSECTION_INSIDE;
        else if (minInAABB || maxInAABB)
            return AABBIntersection::AABB_INTERSECTION_PARTIAL;
        else
            return AABBIntersection::AABB_INTERSECTION_OUTSIDE;
    }

	bool IsPointInSphere(const Vec3& point, const Vec3& sphereCentre, const float radius)
	{
		return glm::distance(sphereCentre, point) <= radius;
	}

    bool IsPointInAABB(const Vec3& point, const AABB& aabb)
    {
        const Vec3 min = aabb.Min();
        const Vec3 max = aabb.Max();

        return min.x <= point.x && point.x <= max.x &&
               min.y <= point.y && point.y <= max.y &&
               min.z <= point.z && point.y <= max.z;
    }
}