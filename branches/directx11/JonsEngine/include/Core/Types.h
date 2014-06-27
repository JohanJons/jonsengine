#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Math.h"

#include <vector>

namespace JonsEngine
{
    /*
     * Definitions of general data types commonly used across subsystems
     */

    enum ColorFormat
    {
        COLOR_FORMAT_RGB = 0,
        COLOR_FORMAT_RGBA
    };

    enum TextureType
    {
        TEXTURE_TYPE_DIFFUSE = 0,
        TEXTURE_TYPE_NORMAL
    };

    typedef uint32_t MeshID;
    typedef uint32_t TextureID;

    const MeshID    INVALID_MESH_ID    = 0;
    const TextureID INVALID_TEXTURE_ID = 0;

    typedef ::DirectX::SimpleMath::Vector2 Vec2;
    typedef ::DirectX::SimpleMath::Vector3 Vec3;
    typedef ::DirectX::SimpleMath::Vector4 Vec4;
    typedef ::DirectX::SimpleMath::Matrix Mat4;
    typedef ::DirectX::SimpleMath::Quaternion Quaternion;
}