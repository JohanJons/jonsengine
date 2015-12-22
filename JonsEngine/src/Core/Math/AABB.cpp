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


    const Vec3& AABB::GetCenter() const
    {
        return mAABBCenter;
    }

    const Vec3& AABB::GetExtent() const
    {
        return mAABBExtent;
    }


    AABB operator*(const Mat4& transform, const AABB& aabb)
    {
        const Vec3 oldMin = aabb.Min();
        const Vec3 oldMax = aabb.Max();
        /*
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

        return AABB(min, max);*/
        Vec4 xa = transform[0] * oldMin.x;
        Vec4 xb = transform[0] * oldMax.x;

        Vec4 ya = transform[1] * oldMin.y;
        Vec4 yb = transform[1] * oldMax.y;

        Vec4 za = transform[2] * oldMin.z;
        Vec4 zb = transform[2] * oldMax.z;

        return AABB(Vec3(MinVal(xa, xb) + MinVal(ya, yb) + MinVal(za, zb) + transform[3]),
                    Vec3(MaxVal(xa, xb) + MaxVal(ya, yb) + MaxVal(za, zb) + transform[3]));
    }

    AABB operator*(const AABB& aabb, const Mat4& transform)
    {
        return transform * aabb;
    }
}