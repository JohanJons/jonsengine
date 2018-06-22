#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstdint>
#include <cstddef>
#include <array>

namespace JonsEngine
{
    /*
     * Definitions of general data types commonly used across subsystems
     */

    enum class TextureType : int
    {
        Diffuse,
        Normal,
		Height,
        Skybox
    };
    
    // TODO: move mesh/texture id definitions
    typedef uint32_t DX11MeshID;
    typedef uint32_t DX11TextureID;

    static const DX11MeshID INVALID_DX11_MESH_ID = 0;
    static const DX11TextureID INVALID_DX11_TEXTURE_ID = 0;

	typedef glm::uvec2 uVec2;
	typedef glm::uvec3 uVec3;
	typedef glm::ivec2 iVec2;
	typedef glm::ivec3 iVec3;
    typedef glm::vec2 Vec2;
    typedef glm::vec3 Vec3;
    typedef glm::vec4 Vec4;
    typedef glm::mat3 Mat3;
    typedef glm::mat4 Mat4;
    typedef glm::fquat Quaternion;

	typedef Vec3 Color;
	static const Color gRed( 1.0f, 0.0f, 0.0f );
	static const Color gGreen( 0.0f, 1.0f, 0.0f );
	static const Color gBlue( 0.0f, 0.0f, 1.0f );
}