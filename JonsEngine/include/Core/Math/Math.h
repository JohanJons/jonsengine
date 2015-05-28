#pragma once

#include "include/Core/Math/KDOP.h"
#include "include/Core/Types.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include <math.h>


namespace JonsEngine
{
    /*
     * Contains various math functions
     */

    typedef KDOP<6> FrustumPlanes;
    typedef std::array<Vec4, 8> FrustumCorners;
     
    const Mat4 gIdentityMatrix(1.0f);


    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float ratio, const float zNear, const float zFar);
    Mat4 OrthographicMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar);
    FrustumCorners GetFrustumCorners(const Mat4& viewFrustum);
    FrustumPlanes GetFrustumPlanes(const Mat4& viewFrustum);

    Vec3 MinVal(const Vec3& v1, const Vec3& v2);
    Vec4 MinVal(const Vec4& v1, const Vec4& v2);
    Vec3 MaxVal(const Vec3& v1, const Vec3& v2);
    Vec4 MaxVal(const Vec4& v1, const Vec4& v2);
}