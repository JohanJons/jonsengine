#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gDirLightFragmentShader =
    "#version 430                                                                                                                   \n \
                                                                                                                                    \n \
    layout(std140) uniform;                                                                                                         \n \
                                                                                                                                    \n \
    uniform UnifDirLight                                                                                                            \n \
    {                                                                                                                               \n \
        mat4 mVPMatrix;                                                                                                             \n \
        vec4 mLightColor;                                                                                                           \n \
        vec4 mLightDir;                                                                                                             \n \
        vec4 mGamma;                                                                                                                \n \
        vec2 mScreenSize;                                                                                                           \n \
    } UnifDirLightPass;                                                                                                             \n \
                                                                                                                                    \n \
    layout (binding = 2) uniform sampler2D unifPositionTexture;                                                                     \n \
    layout (binding = 3) uniform sampler2D unifNormalTexture;                                                                       \n \
    layout (binding = 4) uniform sampler2D unifDiffuseTexture;                                                                      \n \
    layout (binding = 6) uniform sampler2DShadow unifShadowTexture;                                                                 \n \
                                                                                                                                    \n \
    out vec4 fragColor;                                                                                                             \n \
                                                                                                                                    \n \
    void main()                                                                                                                     \n \
    {                                                                                                                               \n \
        vec2 texcoord = gl_FragCoord.xy / UnifDirLightPass.mScreenSize;                                                             \n \
                                                                                                                                    \n \
        vec3 worldPos = texture(unifPositionTexture, texcoord).xyz;                                                                 \n \
        vec3 normal   = normalize(texture(unifNormalTexture, texcoord).xyz);                                                        \n \
        vec3 diffuse  = texture(unifDiffuseTexture, texcoord).xyz;                                                                  \n \
                                                                                                                                    \n \
        vec4 lightClipPos = UnifDirLightPass.mVPMatrix * vec4(worldPos, 1.0);                                                       \n \
        vec3 projCoords   = lightClipPos.xyz / lightClipPos.w;                                                                      \n \
        projCoords.z      = projCoords.z - 0.0005;                                                                                   \n \
                                                                                                                                    \n \
        float visibilty  = texture(unifShadowTexture, projCoords);                                                                  \n \
                                                                                                                                    \n \
        float angleNormal = clamp(dot(normal, UnifDirLightPass.mLightDir.xyz), 0, 1);                                              \n \
                                                                                                                                    \n \
        fragColor = vec4(diffuse, 1.0) * visibilty * angleNormal * UnifDirLightPass.mLightColor;                                    \n \
    }                                                                                                                               \n";
}