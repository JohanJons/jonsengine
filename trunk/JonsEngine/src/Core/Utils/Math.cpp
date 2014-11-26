#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    float signum(float val)
    {
        if (val > 0.0f) return 1.0f;
        if (val < 0.0f) return -1.0f;

        return 0.0f;
    }


    FrustrumPlanes GetFrustrumPlanes(const Mat4& WVPMatrix)
    {
        FrustrumPlanes ret;

        ret[0] = WVPMatrix[3] + WVPMatrix[0];
        ret[1] = WVPMatrix[3] - WVPMatrix[0];
        ret[2] = WVPMatrix[3] - WVPMatrix[1];
        ret[3] = WVPMatrix[3] + WVPMatrix[1];
        ret[4] = WVPMatrix[2];
        ret[5] = WVPMatrix[3] - WVPMatrix[2];

        for (Plane& plane : ret)
            plane = glm::normalize(plane);

        return ret;
    }


    bool DoesAABBIntersectViewFrustrum(const Vec3& aabbCenter, const Vec3& aabbExtent, const Mat4& WVPMatrix)
    {
        FrustrumPlanes planes = GetFrustrumPlanes(WVPMatrix);

        for (const Plane& plane : planes)
        {
            Vec3 signFlip(signum(plane.x), signum(plane.y), signum(plane.z));
            if (glm::dot(aabbCenter + aabbExtent * signFlip, Vec3(plane)) > -plane.w)
                return true;
        }

        return false;
    }


    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float aspectRatio, const float zNear, const float zFar)
    {
        const float fovRadians = glm::radians(fovDegrees);
        const float yScale = 1 / tan(fovRadians / 2.0f);
        const float xScale = yScale / aspectRatio;

        Mat4 ret(0.0f);

        ret[0][0] = xScale;
        ret[1][1] = yScale;
        ret[2][2] = zFar / (zNear - zFar);
        ret[2][3] = -1;
        ret[3][2] = zNear * zFar / (zNear - zFar);

        return ret;
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
}