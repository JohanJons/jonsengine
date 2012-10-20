#pragma once

#include "glm/glm.hpp"

/*
 * Definitions of general data types commonly used across subsystems
 */
namespace JonsEngine
{
    typedef glm::vec2 Vec2;
    typedef glm::vec3 Vec3;
    typedef glm::vec4 Vec4;

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