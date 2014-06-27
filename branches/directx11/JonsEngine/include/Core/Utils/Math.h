#pragma once

#include "include/Core/Types.h"

#include "SimpleMath.h"
#include <math.h>


namespace JonsEngine
{
    /*
     * Contains various math utilities
     */

    Vec2 Add(const Vec2& vec1, const Vec2& vec2);
    Vec3 Add(const Vec3& vec1, const Vec3& vec2);
    Mat3 Add(const Mat3& mat1, const Mat3& mat2);
    Mat4 Add(const Mat4& mat1, const Mat4& mat2);

    Vec2 Multiply(const Vec2& vec1, const Vec2& vec2);
    Vec3 Multiply(const Vec3& vec1, const Vec3& vec2);
    Vec3 Multiply(const Vec3& vec, const float magnitude);
    Vec4 Multiply(const Vec4& vec, const Mat4& mat);
    Mat3 Multiply(const Mat3& mat1, const Mat3& mat2);
    Mat4 Multiply(const Mat4& mat1, const Mat4& mat2);
    Quaternion Multiply(const Quaternion& quat1, const Quaternion& quat2);

    Vec3 Negate(const Vec3& vec);
    Mat4 Inverse(const Mat4& mat);
    Quaternion RotateAxis(const Quaternion& quaternion, const Vec3& rotateVec, const float angle);

    Mat4 Perspective(const float angle, const float aspectRatio, const float nearZ, const float farZ);
    Mat4 Translate(const Mat4& mat, const Vec3& offset);
    Mat4 Scale(const Mat4& mat, const Vec3& scale);
    Mat4 RotationMatrix(const Quaternion& quaternion);

    Mat4 GetIdentityMatrix();
    Quaternion GetIdentityQuaternion();
}