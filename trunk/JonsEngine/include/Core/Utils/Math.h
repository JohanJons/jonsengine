#pragma once

#include "include/Core/Types.h"


namespace JonsEngine
{
    /*
     * Contains various math utilities
     */
    struct Camera;


    Mat4 ScaleTransform(Mat4& transform, const Vec3& scaleVec);
    Mat4 RotateTransform(Mat4& transform, const float angle, const Vec3& rotateVec);
    Mat4 TranslateTransform(Mat4& transform, const Vec3& translateVec);

    Quaternion AngleAxisToQuaternion(const float angle, const Vec3& axis);
    Mat4 QuaternionToMat4(const Quaternion& quat);

    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar);
    Mat4 CreateViewMatrix(const Camera& camera);
    Mat4 CreateViewMatrix(const Vec3& CameraPosition, const Vec3& TargetVector, const Vec3& UpVector);

}