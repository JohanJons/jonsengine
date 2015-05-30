#include "include/Core/Math/Math.h"

#include "include/Core/Math/Plane.h"

namespace JonsEngine
{
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

    FrustumCorners GetFrustumCorners(const Mat4& cameraViewProjMatrix)
    {
        FrustumCorners ret = { Vec4(-1.0f, -1.0f, 0.0f, 1.0f),     // near bottom-left
                               Vec4(-1.0f, 1.0f, 0.0f, 1.0f),      // near top-left
                               Vec4(1.0f, 1.0f, 0.0f, 1.0f),       // near top-right
                               Vec4(1.0f, -1.0f, 0.0f, 1.0f),      // near bottom-right
                               Vec4(-1.0f, -1.0f, 1.0f, 1.0f),     // far bottom-left
                               Vec4(-1.0f, 1.0f, 1.0f, 1.0f),      // far top-left
                               Vec4(1.0f, 1.0f, 1.0f, 1.0f),       // far top-right
                               Vec4(1.0f, -1.0f, 1.0f, 1.0f), };   // far bottom-right

        const Mat4 invVP = glm::inverse(cameraViewProjMatrix);

        for (Vec4& corner : ret)
        {
            corner = invVP * corner;
            corner /= corner.w;
        }

        return ret;
    }

    FrustumPlanes GetFrustumPlanes(const Mat4& viewFrustum)
    {
        return FrustumPlanes
        { {
                Plane(Vec4(viewFrustum[0].w, viewFrustum[1].w, viewFrustum[2].w, viewFrustum[3].w) + Vec4(viewFrustum[0].x, viewFrustum[1].x, viewFrustum[2].x, viewFrustum[3].x)),      // left
                Plane(Vec4(viewFrustum[0].w, viewFrustum[1].w, viewFrustum[2].w, viewFrustum[3].w) - Vec4(viewFrustum[0].x, viewFrustum[1].x, viewFrustum[2].x, viewFrustum[3].x)),      // right
                Plane(Vec4(viewFrustum[0].w, viewFrustum[1].w, viewFrustum[2].w, viewFrustum[3].w) - Vec4(viewFrustum[0].y, viewFrustum[1].y, viewFrustum[2].y, viewFrustum[3].y)),      // top
                Plane(Vec4(viewFrustum[0].w, viewFrustum[1].w, viewFrustum[2].w, viewFrustum[3].w) + Vec4(viewFrustum[0].y, viewFrustum[1].y, viewFrustum[2].y, viewFrustum[3].y)),      // bottom
                Plane(Vec4(viewFrustum[0].z, viewFrustum[1].z, viewFrustum[2].z, viewFrustum[3].z)),                                                                                     // near
                Plane(Vec4(viewFrustum[0].w, viewFrustum[1].w, viewFrustum[2].w, viewFrustum[3].w) - Vec4(viewFrustum[0].z, viewFrustum[1].z, viewFrustum[2].z, viewFrustum[3].z)),      // far
        } };
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
}