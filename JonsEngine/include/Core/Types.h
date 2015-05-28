#pragma once

#include "include/Core/EngineDefs.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <array>

namespace JonsEngine
{
    /*
     * Definitions of general data types commonly used across subsystems
     */

    enum TextureType
    {
        TEXTURE_TYPE_DIFFUSE = 0,
        TEXTURE_TYPE_NORMAL
    };
    
    // TODO: move mesh/texture id definitions
    typedef uint32_t MeshID;
    typedef uint32_t TextureID;

    const MeshID INVALID_MESH_ID = 0;
    const TextureID INVALID_TEXTURE_ID = 0;

    typedef glm::vec2 Vec2;
    typedef glm::vec3 Vec3;
    typedef glm::vec4 Vec4;
    typedef glm::mat3 Mat3;
    typedef glm::mat4 Mat4;
    typedef glm::fquat Quaternion;
}