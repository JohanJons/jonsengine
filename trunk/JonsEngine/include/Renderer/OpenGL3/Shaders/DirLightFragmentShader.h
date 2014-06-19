#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gDirLightFragmentShader =
    "#version 420                                                                                                                   \n \
                                                                                                                                    \n \
    const float DEPTH_BIAS = 0.00005;                                                                                               \n \
                                                                                                                                    \n \
    layout(std140) uniform UnifDirLight                                                                                             \n \
    {                                                                                                                               \n \
        mat4 mVPMatrix[4];                                                                                                          \n \
        mat4 mCamViewMatrix;                                                                                                        \n \
        vec4 mSplitDistance;                                                                                                        \n \
        vec4 mLightColor;                                                                                                           \n \
        vec4 mLightDir;                                                                                                             \n \
        vec4 mGamma;                                                                                                                \n \
        vec2 mScreenSize;                                                                                                           \n \
        int mNumSamples;                                                                                                            \n \
    } UnifDirLightPass;                                                                                                             \n \
                                                                                                                                    \n \
    layout (binding = 2) uniform sampler2DMS unifPositionTexture;                                                                   \n \
    layout (binding = 3) uniform sampler2DMS unifNormalTexture;                                                                     \n \
    layout (binding = 4) uniform sampler2DMS unifDiffuseTexture;                                                                    \n \
    layout (binding = 6) uniform sampler2DArrayShadow unifShadowmap;                                                                \n \
                                                                                                                                    \n \
    out vec4 fragColor;                                                                                                             \n \
                                                                                                                                    \n \
    void main()                                                                                                                     \n \
    {                                                                                                                               \n \
        ivec2 texcoord = ivec2(textureSize(unifDiffuseTexture) * (gl_FragCoord.xy / UnifDirLightPass.mScreenSize));                 \n \
                                                                                                                                    \n \
        vec3 worldPos = vec3(0.0), normal = vec3(0.0), diffuse = vec3(0.0);                                                         \n \
        for (int i = 0; i < UnifDirLightPass.mNumSamples; i++)                                                                      \n \
        {                                                                                                                           \n \
            worldPos += texelFetch(unifPositionTexture, texcoord, i).rgb;                                                           \n \
            normal   += texelFetch(unifNormalTexture, texcoord, i).rgb;                                                             \n \
            diffuse  += texelFetch(unifDiffuseTexture, texcoord, i).rgb;                                                            \n \
        }                                                                                                                           \n \
        worldPos /= UnifDirLightPass.mNumSamples;                                                                                   \n \
        normal   /= UnifDirLightPass.mNumSamples;                                                                                   \n \
        diffuse  /= UnifDirLightPass.mNumSamples;                                                                                   \n \
        normal = normalize(normal);                                                                                                 \n \
                                                                                                                                    \n \
        vec4 camPos = UnifDirLightPass.mCamViewMatrix * vec4(worldPos, 1.0);                                                        \n \
                                                                                                                                    \n \
        int index = 3;                                                                                                              \n \
        if (camPos.z > UnifDirLightPass.mSplitDistance.x)                                                                           \n \
            index = 0;                                                                                                              \n \
        else if (camPos.z > UnifDirLightPass.mSplitDistance.y)                                                                      \n \
            index = 1;                                                                                                              \n \
        else if (camPos.z > UnifDirLightPass.mSplitDistance.z)                                                                      \n \
            index = 2;                                                                                                              \n \
                                                                                                                                    \n \
        vec4 projCoords = UnifDirLightPass.mVPMatrix[index] * vec4(worldPos, 1.0);                                                  \n \
        projCoords.w    = projCoords.z - DEPTH_BIAS;                                                                                \n \
        projCoords.z    = float(index);                                                                                             \n \
        float visibilty = texture(unifShadowmap, projCoords);                                                                       \n \
                                                                                                                                    \n \
        float angleNormal = clamp(dot(normal, UnifDirLightPass.mLightDir.xyz), 0, 1);                                               \n \
                                                                                                                                    \n \
        fragColor = vec4(diffuse, 1.0) * visibilty * angleNormal * UnifDirLightPass.mLightColor;                                    \n \
    }                                                                                                                               \n";
}