#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gAmbientFragmentShader =
    "#version 430                                                                                                                   \n \
                                                                                                                                    \n \
    layout(std140) uniform;                                                                                                         \n \
                                                                                                                                    \n \
    uniform UnifAmbient                                                                                                             \n \
    {                                                                                                                               \n \
        mat4 mWVPMatrix;                                                                                                            \n \
        vec4 mLightColor;                                                                                                           \n \
        vec4 mGamma;                                                                                                                \n \
        vec2 mScreenSize;                                                                                                           \n \
    } UnifAmbientPass;                                                                                                              \n \
                                                                                                                                    \n \
    layout (binding = 2) uniform sampler2D unifPositionTexture;                                                                     \n \
    layout (binding = 3) uniform sampler2D unifNormalTexture;                                                                       \n \
    layout (binding = 4) uniform sampler2D unifDiffuseTexture;                                                                      \n \
                                                                                                                                    \n \
    out vec4 fragColor;                                                                                                             \n \
                                                                                                                                    \n \
    void main()                                                                                                                     \n \
    {                                                                                                                               \n \
        vec2 texcoord = gl_FragCoord.xy / UnifAmbientPass.mScreenSize;                                                              \n \
                                                                                                                                    \n \
        vec3 worldPos = texture(unifPositionTexture, texcoord).xyz;                                                                 \n \
        vec3 normal   = normalize(texture(unifNormalTexture, texcoord).xyz);                                                        \n \
        vec3 diffuse  = texture(unifDiffuseTexture, texcoord).xyz;                                                                  \n \
                                                                                                                                    \n \
        fragColor = vec4(diffuse, 1.0) * UnifAmbientPass.mLightColor;                                                               \n \
    }                                                                                                                               \n";
}