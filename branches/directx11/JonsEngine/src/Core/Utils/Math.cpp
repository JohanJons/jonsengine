#include "include/Core/Types.h"

#include <math.h>

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

        /*
        ret[0][0] = static_cast<T>(2) / (right - left);
        ret[1][1] = static_cast<T>(2) / (top - bottom);
        ret[2][2] = -T(2) / (zFar - zNear);
        ret[3][0] = -(right + left) / (right - left);
        ret[3][1] = -(top + bottom) / (top - bottom);
        ret[3][2] = -(zFar + zNear) / (zFar - zNear);
        */
		//return Result;
        

        return ret;
    }
}