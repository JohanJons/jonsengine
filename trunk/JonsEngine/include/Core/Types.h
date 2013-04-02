#pragma once

#include <vector>
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
     typedef glm::mat3 Mat3;
     typedef glm::mat4 Mat4;
     typedef glm::fquat Quaternion;
 
     const std::vector<float> _GetTypeValues(const float* valuePtr, size_t numValues, size_t numPadding = 0);
     inline const std::vector<float> GetTypeValues(const Vec2& vec)               { return _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float));      }        
     inline const std::vector<float> GetTypeValues(const Vec3& vec)               { return _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float));      }
     inline const std::vector<float> GetTypeValues(const Vec4& vec)               { return _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float));      }
     inline const std::vector<float> GetTypeValues(const Mat3& mat)               { return _GetTypeValues(glm::value_ptr(mat), sizeof(mat)/sizeof(float));      }
     inline const std::vector<float> GetTypeValues(const Mat4& mat)               { return _GetTypeValues(glm::value_ptr(mat), sizeof(mat)/sizeof(float));      }
     inline const std::vector<float> GetTypeValues(const Quaternion& quat)        { return _GetTypeValues(glm::value_ptr(quat), sizeof(quat)/sizeof(float));    }
 
     inline const std::vector<float> _GetTypeValues(const float* valuePtr, size_t numValues, size_t numPadding)
     {
         std::vector<float> ret; 
         for(size_t i = 0; i < numValues; i++)
             ret.push_back(*(valuePtr + i));
 
         // TODO: find a good system to match the potential padding required by GLSL

         return ret;
     }
}