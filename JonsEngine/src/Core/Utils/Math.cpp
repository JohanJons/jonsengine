#include "include/Core/Utils/Math.h"

#include <cmath>

namespace JonsEngine
{
	AABB::AABB(const Vec3& center, const Vec3& extent) : mAABBCenter(center, 1.0f), mAABBExtent(extent, 1.0f)
    {
    }

	AABB::AABB(const Vec4& center, const Vec4& extent) : mAABBCenter(center), mAABBExtent(extent)
	{
	}

    AABB& AABB::operator*=(const Mat4& transform)
	{
		mAABBCenter = transform * mAABBCenter;
		mAABBExtent = transform * mAABBExtent;

        return *this;
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
            d = glm::dot(plane, aabb.mAABBCenter);
            r = glm::dot(glm::abs(plane), aabb.mAABBExtent);

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
        const Vec3 min = Vec3(sphereCentre.x - sphereRadius, sphereCentre.y - sphereRadius, sphereCentre.z - sphereRadius);
        const Vec3 max = Vec3(sphereCentre.x + sphereRadius, sphereCentre.y + sphereRadius, sphereCentre.z + sphereRadius);

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

    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float aspectRatio, const float zNear, const float zFar)
    {
        const float fovRadians = glm::radians(fovDegrees);
        const float yScale = 1 / tan(fovRadians / 2.0f);
        const float xScale = yScale / aspectRatio;

        Mat4 ret(0.0f);

        ret[0][0] = xScale;
        ret[1][1] = yScale;
        ret[2][2] = zFar / (zNear - zFar);
        ret[2][3] = -1;
        ret[3][2] = zNear * zFar / (zNear - zFar);

        return ret;
    }

    Mat4 OrthographicMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar)
    {
        Mat4 ret(1.0f);

        ret[0][0] = 2.0f / (right - left);
        ret[1][1] = 2.0f / (top - bottom);
        ret[2][2] = 1.0f / (zNear - zFar);
        ret[3][0] = (left + right) / (left - right);
        ret[3][1] = (top + bottom) / (bottom - top);
        ret[3][2] = (zNear) / (zNear - zFar);

        return ret;
    }


    Vec3 MinVal(const Vec3& v1, const Vec3& v2)
    {
        return Vec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
    }

    Vec3 MaxVal(const Vec3& v1, const Vec3& v2)
    {
        return Vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
    }
}