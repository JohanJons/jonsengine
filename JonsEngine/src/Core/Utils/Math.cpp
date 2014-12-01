#include "include/Core/Utils/Math.h"

#include <cmath>

namespace JonsEngine
{
    /*
     * Returns worldspace frustrum planes
     */

    bool extentSignedTest(const Vec4& p, const Vec3& center, const Vec3& extent)
    {
        return (glm::dot(Vec3(p), center) + glm::dot(glm::abs(Vec3(p)), extent) < -p.w);
    }

    bool isAABBInFrustumReference(const Vec3& center, const Vec3& extent, const Mat4& frustumMatrix)
    {
        const Vec4 rowX(frustumMatrix[0].x, frustumMatrix[1].x, frustumMatrix[2].x, frustumMatrix[3].x);
        const Vec4 rowY(frustumMatrix[0].y, frustumMatrix[1].y, frustumMatrix[2].y, frustumMatrix[3].y);
        const Vec4 rowZ(frustumMatrix[0].z, frustumMatrix[1].z, frustumMatrix[2].z, frustumMatrix[3].z);
        const Vec4 rowW(frustumMatrix[0].w, frustumMatrix[1].w, frustumMatrix[2].w, frustumMatrix[3].w);

        // Left and right planes              
        if (extentSignedTest(rowW + rowX, center, extent))
            return false;

        if (extentSignedTest(rowW - rowX, center, extent))
            return false;

        // Bottom and top planes
        if (extentSignedTest(rowW + rowY, center, extent))
            return false;

        if (extentSignedTest(rowW - rowY, center, extent))
            return false;

        // Near and far planes
        if (extentSignedTest(rowW + rowZ, center, extent))
            return false;

        if (extentSignedTest(rowW - rowZ, center, extent))
            return false;

        return true;
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