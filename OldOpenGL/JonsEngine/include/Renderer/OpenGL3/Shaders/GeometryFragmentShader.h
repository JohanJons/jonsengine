#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gGeometryFragmentShader = 
    "#version 420                                                                                                           \n \
                                                                                                                            \n \
    layout(std140) uniform UnifGeometry                                                                                     \n \
    {                                                                                                                       \n \
        mat4 mWVPMatrix;                                                                                                    \n \
        mat4 mWorldMatrix;                                                                                                  \n \
        bool mHasDiffuseTexture;                                                                                            \n \
        bool mHasNormalTexture;                                                                                             \n \
        float mTextureTilingFactor;                                                                                         \n \
    } UnifGeometryPass;                                                                                                     \n \
                                                                                                                            \n \
    layout (binding = 0) uniform sampler2D unifDiffuseTexture;                                                              \n \
    layout (binding = 1) uniform sampler2D unifNormalTexture;                                                               \n \
                                                                                                                            \n \
    in vec3 frag_position;                                                                                                  \n \
    in vec3 frag_normal;                                                                                                    \n \
    in vec2 frag_texcoord;                                                                                                  \n \
    in vec3 frag_tangent;                                                                                                   \n \
    in vec3 frag_bitangent;                                                                                                 \n \
                                                                                                                            \n \
    layout(location = 0) out vec3 outPosition;                                                                              \n \
    layout(location = 1) out vec3 outNormal;                                                                                \n \
    layout(location = 2) out vec3 outDiffuse;                                                                               \n \
                                                                                                                            \n \
    void main()                                                                                                             \n \
    {                                                                                                                       \n \
        outPosition = frag_position;                                                                                        \n \
                                                                                                                            \n \
        if (UnifGeometryPass.mHasNormalTexture)                                                                             \n \
        {                                                                                                                   \n \
            vec3 normalTangentSpace  = normalize(texture(unifNormalTexture, frag_texcoord).xyz * 2.0 - 1.0);                \n \
            mat3 tangentToWorldSpace = mat3(normalize(frag_tangent), normalize(frag_bitangent), normalize(frag_normal));    \n \
                                                                                                                            \n \
            outNormal = tangentToWorldSpace * normalTangentSpace;                                                           \n \
        }                                                                                                                   \n \
        else                                                                                                                \n \
            outNormal = normalize(frag_normal);                                                                             \n \
                                                                                                                            \n \
        if (UnifGeometryPass.mHasDiffuseTexture)                                                                            \n \
            outDiffuse = texture(unifDiffuseTexture, frag_texcoord).xyz;                                                    \n \
        else                                                                                                                \n \
            outDiffuse = vec3(1.0);                                                                                         \n \
    }                                                                                                                       \n";
}