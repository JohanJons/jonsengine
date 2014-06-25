#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gGeometryVertexShader =
    "#version 420                                                                           \n \
                                                                                            \n \
    layout(std140) uniform UnifGeometry                                                     \n \
    {                                                                                       \n \
        mat4 mWVPMatrix;                                                                    \n \
        mat4 mWorldMatrix;                                                                  \n \
        bool mHasDiffuseTexture;                                                            \n \
        bool mHasNormalTexture;                                                             \n \
        float mTextureTilingFactor;                                                         \n \
    } UnifGeometryPass;                                                                     \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
    layout(location = 1) in vec3 vert_normal;                                               \n \
    layout(location = 2) in vec2 vert_texcoord;                                             \n \
    layout(location = 3) in vec3 vert_tangent;                                              \n \
    layout(location = 4) in vec3 vert_bitangent;                                            \n \
                                                                                            \n \
    out vec3 frag_position;                                                                 \n \
    out vec3 frag_normal;                                                                   \n \
    out vec2 frag_texcoord;                                                                 \n \
    out vec3 frag_tangent;                                                                  \n \
    out vec3 frag_bitangent;                                                                \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = UnifGeometryPass.mWVPMatrix * vec4(vert_position, 1.0);               \n \
                                                                                            \n \
        frag_position  = (UnifGeometryPass.mWorldMatrix * vec4(vert_position, 1.0)).xyz;    \n \
        frag_normal    = (UnifGeometryPass.mWorldMatrix * vec4(vert_normal, 0.0)).xyz;      \n \
        frag_texcoord  = UnifGeometryPass.mTextureTilingFactor * vert_texcoord;             \n \
        frag_tangent   = (UnifGeometryPass.mWorldMatrix * vec4(vert_tangent, 0.0)).xyz;     \n \
        frag_bitangent = (UnifGeometryPass.mWorldMatrix * vec4(vert_bitangent, 0.0)).xyz;   \n \
    }                                                                                       \n";
}