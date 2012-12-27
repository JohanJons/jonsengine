#pragma once

#include "include/Core/Utils/Types.h"


namespace JonsEngine
{
    /*
     * Contains various math utilities
     */

    Mat4 ScaleTransform(Mat4& transform, const Vec3& scaleVec);
    Mat4 RotateTransform(Mat4& transform, const float angle, const Vec3& rotateVec);
    Mat4 TranslateTransform(Mat4& transform, const Vec3& translateVec);

    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar);
}