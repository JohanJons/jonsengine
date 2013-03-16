#pragma once

#include "include/Core/Types.h"


namespace JonsEngine
{
    /*
     * Contains various math utilities
     */
    class Camera;


    Mat4 Scale(const Mat4& matrix, const Vec3& scaleVec);
    Mat4 Rotate(const Mat4& matrix, const float angle, const Vec3& rotateVec);
    Mat4 Translate(const Mat4& matrix, const Vec3& translateVec);
    Mat4 Inverse(const Mat4& matrix);

    Quaternion AngleAxisToQuaternion(const float angle, const Vec3& axis);
    Mat4 QuaternionToMat4(const Quaternion& quat);

    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar);
}