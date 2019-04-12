#include "AABB.h"
#include "MathUtils.h"
#include "JonsPackage.h"

#include <cmath>

namespace JonsEngine
{
	AABB AABB::gUnitAABB = AABB( { 1.0f, 1.0f, 1.0f }, { -1.0f, -1.0f, -1.0f } );
	AABB AABB::gUnitQuadAABB = AABB( { 1.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, -1.0f } );

	AABB::AABB(const PackageAABB& pkgAABB) :
		AABB(pkgAABB.mMinBounds, pkgAABB.mMaxBounds)
	{
	}

    AABB::AABB(const Vec3& minBounds, const Vec3& maxBounds) :
		mAABBCenter(0.5f * (minBounds + maxBounds)),
		mAABBExtent(0.5f * abs(maxBounds - minBounds))
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
        const Vec4 newMin = transform * Vec4( aabb.Min(), 1.0f );
		const Vec4 newMax = transform * Vec4( aabb.Max(), 1.0f );

		return AABB( Vec3( newMin ), Vec3( newMax ) );
    }

    AABB operator*(const AABB& aabb, const Mat4& transform)
    {
        return transform * aabb;
    }
}
