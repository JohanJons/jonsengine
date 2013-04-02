#include "include/Core/Utils/Math.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"


namespace JonsEngine
{
    Mat4 Scale(const Mat4& matrix, const Vec3& scaleVec)
    {
        return glm::scale(matrix, scaleVec);
    }

    Mat4 Rotate(const Mat4& matrix, const float angle, const Vec3& rotateVec)
    {
        return glm::rotate(matrix, angle, rotateVec);
    }

    Mat4 Translate(const Mat4& matrix, const Vec3& translateVec)
    {
        return glm::translate(matrix, translateVec);
    }

    Mat4 Inverse(const Mat4& matrix)
    {
        return glm::inverse(matrix);
    }


    Quaternion AngleAxisToQuaternion(const float angle, const Vec3& axis)
    {
        return glm::angleAxis(angle, axis);
    }

    Mat4 QuaternionToMat4(const Quaternion& quat)
    {
        return glm::toMat4(quat);
    }


    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar)
    {
        return glm::perspective(fov, aspect, zNear, zFar);
    }
}