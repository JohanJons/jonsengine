#include "include/Core/Utils/Math.h"
#include "include/Scene/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace JonsEngine
{
    Mat4 ScaleTransform(Mat4& transform, const Vec3& scaleVec)
    {
        return glm::scale(transform, scaleVec);
    }

    Mat4 RotateTransform(Mat4& transform, const float angle, const Vec3& rotateVec)
    {
        return glm::rotate(transform, angle, rotateVec);
    }

    Mat4 TranslateTransform(Mat4& transform, const Vec3& translateVec)
    {
        return glm::translate(transform, translateVec);
    }


    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar)
    {
        return glm::perspective(fov, aspect, zNear, zFar);
    }

    Mat4 CreateViewMatrix(const Camera& camera)
    {
        return glm::lookAt(camera.CameraPosition, camera.TargetVector, camera.UpVector);
    }

    Mat4 CreateViewMatrix(const Vec3& CameraPosition, const Vec3& TargetVector, const Vec3& UpVector)
    {
        return glm::lookAt(CameraPosition, TargetVector, UpVector);
    }

}