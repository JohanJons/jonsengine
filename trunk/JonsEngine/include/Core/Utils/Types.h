#pragma once

#include "include/Core/Containers/vector.h"

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

    const vector<float> _GetTypeValues(const float* valuePtr, size_t numValues);
    inline const vector<float> GetTypeValues(const Vec2& vec)        { return _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float)); }        
    inline const vector<float> GetTypeValues(const Vec3& vec)        { return _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float)); }
    inline const vector<float> GetTypeValues(const Vec4& vec)        { return _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float)); }
    inline const vector<float> GetTypeValues(const Mat4& mat)        { return _GetTypeValues(glm::value_ptr(mat), sizeof(mat)/sizeof(float)); }

    inline const vector<float> _GetTypeValues(const float* valuePtr, size_t numValues)
    {
        vector<float> ret; 
        for(size_t i = 0; i < numValues; i++)
            ret.push_back(*(valuePtr + i));

        return ret;
    }
}