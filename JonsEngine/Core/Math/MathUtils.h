#pragma once

#include "Core/Math/Plane.h"
#include "Core/Types.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/compatibility.hpp"
#include <math.h>
#include <cmath>
#include <array>


namespace JonsEngine
{
    /*
     * Contains various math functions
     */

    static const Mat4 gIdentityMatrix(1.0f);


    Mat4 PerspectiveMatrixFov(const float fovDegrees, const float ratio, const float zNear, const float zFar);
    Mat4 OrthographicMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar);

    Vec3 MinVal(const Vec3& v1, const Vec3& v2);
    Vec4 MinVal(const Vec4& v1, const Vec4& v2);
    Vec3 MaxVal(const Vec3& v1, const Vec3& v2);
    Vec4 MaxVal(const Vec4& v1, const Vec4& v2);

    float PerspectiveNearPlane(const float zNear, const float zFar);
    float PerspectiveFarPlane(const float zNear, const float zFar);

    bool IsEqual(const float val1, const float val2);
    bool IsEqual(const double val1, const double val2);

	template<typename ValueType>
	float Normalize( ValueType val );


	template<typename ValueType>
	float Normalize( ValueType val )
	{
		ValueType minVal = std::numeric_limits<ValueType>::min(), maxVal = std::numeric_limits<ValueType>::max();
		return ( val - minVal ) / static_cast<float>( maxVal - minVal );
	}
}