#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gDirLightFragmentDebugShader =
    "#version 420                                                                                                                   \n \
                                                                                                                                    \n \
    const float DEPTH_BIAS = 0.00005;                                                                                               \n \
                                                                                                                                    \n \
    layout(std140) uniform UnifDirLight                                                                                                     \n \
    {                                                                                                                               \n \
        mat4 mVPMatrix[4];                                                                                                          \n \
        mat4 mCamViewMatrix;    \n \
        vec4 mSplitDistance;                                                                                                    \n \
        vec4 mLightColor;                                                                                                           \n \
        vec4 mLightDir;                                                                                                             \n \
        vec4 mGamma;                                                                                                                \n \
        vec2 mScreenSize;                                                                                                           \n \
    } UnifDirLightPass;                                                                                                             \n \
                                                                                                                                    \n \
    layout (binding = 2) uniform sampler2D unifPositionTexture;                                                                     \n \
    layout (binding = 3) uniform sampler2D unifNormalTexture;                                                                       \n \
    layout (binding = 4) uniform sampler2D unifDiffuseTexture;                                                                      \n \
    layout (binding = 6) uniform sampler2DArrayShadow unifShadowTexture;                                                            \n \
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
         vec4 worldPos2 =     UnifDirLightPass.mCamViewMatrix * vec4(worldPos, 1.0);                                                 \n \
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);  int index = 3;                                                                        \n \
        if (worldPos2.z > UnifDirLightPass.mSplitDistance.x)                                                                           \n \
             fragColor = vec4(1.0, 0.0, 0.0, 1.0); //  index = 0;                                                                                                              \n \
        else if (worldPos2.z > UnifDirLightPass.mSplitDistance.y)                                                                      \n \
             fragColor = vec4(0.0, 1.0, 0.0, 1.0); // index = 1;                                                                                                            \n \
        else if (worldPos2.z > UnifDirLightPass.mSplitDistance.z)                                                                      \n \
             fragColor = vec4(0.0, 0.0, 1.0, 1.0); // index = 2;                                                                                                              \n \
                                                                                                                                  \n \
        vec4 projCoords = UnifDirLightPass.mVPMatrix[index] * vec4(worldPos, 1.0);                                                  \n \
        projCoords.w    = projCoords.z - DEPTH_BIAS;                                                                                \n \
        projCoords.z    = float(index);                                                                                             \n \
        float visibilty = texture(unifShadowTexture, projCoords);                                                                   \n \
                                                                                                                                    \n \
        float angleNormal = clamp(dot(normal, UnifDirLightPass.mLightDir.xyz), 0, 1);                                               \n \
                                                                                                                                    \n \
      //  fragColor = vec4(diffuse, 1.0) * visibilty * angleNormal * UnifDirLightPass.mLightColor;                                    \n \
    }                                                                                                                               \n";
}