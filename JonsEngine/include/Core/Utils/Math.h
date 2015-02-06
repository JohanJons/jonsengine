#pragma once

#include "include/Core/Types.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include <array>
#include <math.h>


namespace JonsEngine
{
    /*
     * Contains various math utilities
     */
     
    const Mat4 gIdentityMatrix(1.0f);
    
    enum class AABBIntersection
    {
        AABB_INTERSECTION_INSIDE,
        AABB_INTERSECTION_PARTIAL,
        AABB_INTERSECTION_OUTSIDE
    };
    
    AABBIntersection IsAABBInFrustum(const Vec3& center, const Vec3& extent, const Mat4& frustumMatrix);
	AABBIntersection IsAABBInSphere(const Vec3& center, const Vec3& extent, const Vec3& sphereCentre, const float sphereRadius);

    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float ratio, const float zNear, const float zFar);
    Mat4 OrthographicMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar);

    Vec3 MinVal(const Vec3& v1, const Vec3& v2);
    Vec3 MaxVal(const Vec3& v1, const Vec3& v2);
}