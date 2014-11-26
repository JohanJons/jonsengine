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

    bool DoesAABBIntersectViewFrustrum(const Vec3& aabbCenter, const Vec3& aabbExtent, const Mat4& WVPMatrix);

    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float ratio, const float zNear, const float zFar);
    Mat4 OrthographicMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar);
}