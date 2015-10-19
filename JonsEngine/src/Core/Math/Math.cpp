#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float aspectRatio, const float zNear, const float zFar)
    {
        const float fovRadians = glm::radians(fovDegrees);
        const float yScale = 1 / tan(fovRadians / 2.0f);
        const float xScale = yScale / aspectRatio;
        const float nearPlane = PerspectiveNearPlane(zNear, zFar);
        const float farPlane = PerspectiveFarPlane(zNear, zFar);

        return Mat4(xScale, 0.0f, 0.0f, 0.0f,
                    0.0f, yScale, 0.0f, 0.0f,
                    0.0f, 0.0f, nearPlane, -1.0f,
                    0.0f, 0.0f, farPlane, 0.0f);
    }

    Mat4 OrthographicMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar)
    {
        Mat4 ret(1.0f);

        ret[0][0] = 2.0f / (right - left);
        ret[1][1] = 2.0f / (top - bottom);
        ret[2][2] = 1.0f / (zNear - zFar);
        ret[3][0] = (left + right) / (left - right);
        ret[3][1] = (top + bottom) / (bottom - top);
        ret[3][2] = (zNear) / (zNear - zFar);

        return ret;
    }


    Vec3 MinVal(const Vec3& v1, const Vec3& v2)
    {
        return Vec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
    }

    Vec4 MinVal(const Vec4& v1, const Vec4& v2)
    {
        return Vec4(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z), std::min(v1.w, v2.w));
    }

    Vec3 MaxVal(const Vec3& v1, const Vec3& v2)
    {
        return Vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
    }

    Vec4 MaxVal(const Vec4& v1, const Vec4& v2)
    {
        return Vec4(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z), std::max(v1.w, v2.w));
    }


    float PerspectiveNearPlane(const float zNear, const float zFar)
    {
        return zFar / (zNear - zFar);
    }

    float PerspectiveFarPlane(const float zNear, const float zFar)
    {
        return zNear * PerspectiveNearPlane(zNear, zFar);
    }
}