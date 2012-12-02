#pragma once

#include "glm/glm.hpp"


namespace JonsEngine
{
    /*
     * Definitions of general data types commonly used across subsystems
     */

    typedef glm::vec2 Vec2;
    typedef glm::vec3 Vec3;
    typedef glm::vec4 Vec4;

    typedef glm::mat4 Mat4;

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