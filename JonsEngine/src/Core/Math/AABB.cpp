#include "include/Core/Math/AABB.h"
#include "include/Core/Math/Math.h"

#include <cmath>

namespace JonsEngine
{
    AABB::AABB(const Vec3& minBounds, const Vec3& maxBounds) : mAABBCenter(0.5f * (minBounds + maxBounds)), mAABBExtent(0.5f * (maxBounds - minBounds))
    {
    }


    /*AABB& AABB::operator*=(const Mat4& transform)
	{
        
        
        var xa = m.Right * boundingBox.Min.X;
        var xb = m.Right * boundingBox.Max.X;
 
        var ya = m.Up * boundingBox.Min.Y;
        var yb = m.Up * boundingBox.Max.Y;
 
        var za = m.Backward * boundingBox.Min.Z;
        var zb = m.Backward * boundingBox.Max.Z;
 
        return new BoundingBox(
            Vector3.Min(xa, xb) + Vector3.Min(ya, yb) + Vector3.Min(za, zb) + m.Translation,
            Vector3.Max(xa, xb) + Vector3.Max(ya, yb) + Vector3.Max(za, zb) + m.Translation
        );

        

        const Vec3 min = Min();
        const Vec3 max = Max();

        Vec4 xa = transform[0] * min.x;
        Vec4 xb = transform[0] * max.x;

        Vec4 ya = transform[1] * min.y;
        Vec4 yb = transform[1] * max.y;

        Vec4 za = transform[2] * min.z;
        Vec4 zb = transform[2] * max.z;

       // mAABBCenter = 0.5f * ()


        // TODO: perhaps this can be simplified

        const Vec4 transformedMin = transform * Vec4(min.x, min.y, min.z, 1.0f);
        const Vec4 transformedMax = transform * Vec4(max.x, max.y, max.z, 1.0f);

        mAABBCenter.x = 0.5f * (transformedMin.x + transformedMax.x);
        mAABBCenter.y = 0.5f * (transformedMin.y + transformedMax.y);
        mAABBCenter.z = 0.5f * (transformedMin.z + transformedMax.z);

        mAABBExtent.x = 0.5f * (transformedMax.x - transformedMin.x);
        mAABBExtent.y = 0.5f * (transformedMax.y - transformedMin.y);
        mAABBExtent.z = 0.5f * (transformedMax.z - transformedMin.z);

        return *this;
	}*/


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

        /*
         var xa = m.Right * boundingBox.Min.X;
        var xb = m.Right * boundingBox.Max.X;
 
        var ya = m.Up * boundingBox.Min.Y;
        var yb = m.Up * boundingBox.Max.Y;
 
        var za = m.Backward * boundingBox.Min.Z;
        var zb = m.Backward * boundingBox.Max.Z;
 
        return new BoundingBox(
            Vector3.Min(xa, xb) + Vector3.Min(ya, yb) + Vector3.Min(za, zb) + m.Translation,
            Vector3.Max(xa, xb) + Vector3.Max(ya, yb) + Vector3.Max(za, zb) + m.Translation
        );
        */
        const Vec3 oldMin = aabb.Min();
        const Vec3 oldMax = aabb.Max();
        float av, bv;
        int   i, j;


        //Box new_box(mat.m[12], mat.m[13], mat.m[14], mat.m[12], mat.m[13], mat.m[14]);

        Vec3 min(transform[3].x, transform[3].y, transform[3].z);
        Vec3 max(transform[3].x, transform[3].y, transform[3].z);
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
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


        Vec4 xa = transform[0] * min.x;
        Vec4 xb = transform[0] * max.x;

        Vec4 ya = transform[1] * min.y;
        Vec4 yb = transform[1] * max.y;

        Vec4 za = transform[2] * min.z;
        Vec4 zb = transform[2] * max.z;

        Vec4 minzz = MinVal(xa, xb) + MinVal(ya, yb) + MinVal(za, zb) + transform[3];
        Vec4 maxzz = MaxVal(xa, xb) + MaxVal(ya, yb) + MaxVal(za, zb) + transform[3];


        return AABB(Vec3(MinVal(xa, xb) + MinVal(ya, yb) + MinVal(za, zb) + transform[3]),
                    Vec3(MaxVal(xa, xb) + MaxVal(ya, yb) + MaxVal(za, zb) + transform[3]));
    }

    AABB operator*(const AABB& aabb, const Mat4& transform)
    {
        return transform * aabb;
    }
}