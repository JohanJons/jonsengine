#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace JonsEngine
{
    /*
     * Definitions of general data types commonly used across subsystems
     */

    typedef glm::vec2 Vec2;
    typedef glm::vec3 Vec3;
    typedef glm::vec4 Vec4;

    typedef glm::mat4 Mat4;

    inline const float* GetTypeValue(Vec2 vec)         { return glm::value_ptr(vec); }
    inline const float* GetTypeValue(Vec3 vec)         { return glm::value_ptr(vec); }
    inline const float* GetTypeValue(Vec4 vec)         { return glm::value_ptr(vec); }
    inline const float* GetTypeValue(Mat4 mat)         { return glm::value_ptr(mat); }

    struct Line
    {
        Vec3 vecA;
        Vec3 vecB;
    };

    struct Triangle
    {
        Vec3 vecA;
        Vec3 vecB;
        Vec3 vecC;
    };

}