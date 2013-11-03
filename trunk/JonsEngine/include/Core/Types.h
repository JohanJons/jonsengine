#pragma once

#include "include/Core/EngineDefs.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

namespace JonsEngine
{
    /*
     * Definitions of general data types commonly used across subsystems
     */

    enum TextureFormat
    {
        TEXTURE_FORMAT_UNKNOWN = 0,
        TEXTURE_FORMAT_RGB,
        TEXTURE_FORMAT_RGBA
    };

    enum TextureType
    {
        TEXTURE_TYPE_UNKNOWN = 0,
        TEXTURE_TYPE_DIFFUSE,
        TEXTURE_TYPE_NORMAL
    };

    typedef uint32_t MeshID;
    typedef uint32_t TextureID;

    const MeshID    INVALID_MESH_ID    = 0;
    const TextureID INVALID_TEXTURE_ID = 0;

    typedef glm::vec2  Vec2;
    typedef glm::vec3  Vec3;
    typedef glm::vec4  Vec4;
    typedef glm::mat3  Mat3;
    typedef glm::mat4  Mat4;
    typedef glm::fquat Quaternion;
 
    void _GetTypeValues(const float* valuePtr, size_t numValues, std::vector<float>& buffer);
    inline void GetTypeValues(const Vec2& vec, std::vector<float>& buffer)               { _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float), buffer);      }        
    inline void GetTypeValues(const Vec3& vec, std::vector<float>& buffer)               { _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float), buffer);      }
    inline void GetTypeValues(const Vec4& vec, std::vector<float>& buffer)               { _GetTypeValues(glm::value_ptr(vec), sizeof(vec)/sizeof(float), buffer);      }
    inline void GetTypeValues(const Mat3& mat, std::vector<float>& buffer)               { _GetTypeValues(glm::value_ptr(mat), sizeof(mat)/sizeof(float), buffer);      }
    inline void GetTypeValues(const Mat4& mat, std::vector<float>& buffer)               { _GetTypeValues(glm::value_ptr(mat), sizeof(mat)/sizeof(float), buffer);      }
    inline void GetTypeValues(const Quaternion& quat, std::vector<float>& buffer)        { _GetTypeValues(glm::value_ptr(quat), sizeof(quat)/sizeof(float), buffer);    }
 
    inline void _GetTypeValues(const float* valuePtr, size_t numValues, std::vector<float>& buffer)
    {
        for(size_t i = 0; i < numValues; i++)
            buffer.push_back(*(valuePtr + i));
 
        // TODO: find a good system to match the potential padding required by GLSL
    }
}