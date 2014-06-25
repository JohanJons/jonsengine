#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gShadingFragmentShader = 
    "#version 420                                                                                                                           \n \
                                                                                                                                            \n \
    layout(std140) uniform UnifPointLight                                                                                                   \n \
    {                                                                                                                                       \n \
        mat4 mWVPMatrix;                                                                                                                    \n \
        vec4 mLightColor;                                                                                                                   \n \
        vec4 mLightPos;                                                                                                                     \n \
        vec4 mGamma;                                                                                                                        \n \
        vec2 mScreenSize;                                                                                                                   \n \
                                                                                                                                            \n \
        float mLightIntensity;                                                                                                              \n \
        float mMaxDistance;                                                                                                                 \n \
        int mNumSamples;                                                                                                                    \n \
    } UnifPointLightPass;                                                                                                                   \n \
                                                                                                                                            \n \
    layout (binding = 2) uniform sampler2DMS unifPositionTexture;                                                                           \n \
    layout (binding = 3) uniform sampler2DMS unifNormalTexture;                                                                             \n \
    layout (binding = 4) uniform sampler2DMS unifDiffuseTexture;                                                                            \n \
    layout (binding = 7) uniform samplerCube unifShadowmap;                                                                                 \n \
                                                                                                                                            \n \
    out vec4 fragColor;                                                                                                                     \n \
                                                                                                                                            \n \
    float VectorToDepthValue(vec3 Vec)                                                                                                      \n \
    {                                                                                                                                       \n \
        vec3 AbsVec = abs(Vec);                                                                                                             \n \
        float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));                                                                          \n \
                                                                                                                                            \n \
        const float f = 100.0;                                                                                                              \n \
        const float n = 0.1;                                                                                                                \n \
        float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;                                                           \n \
                                                                                                                                            \n \
        return (NormZComp + 1.0) * 0.5;                                                                                                     \n \
    }                                                                                                                                       \n \
                                                                                                                                            \n \
    void main()                                                                                                                             \n \
    {                                                                                                                                       \n \
        ivec2 texcoord = ivec2(textureSize(unifDiffuseTexture) * (gl_FragCoord.xy / UnifPointLightPass.mScreenSize));                       \n \
                                                                                                                                            \n \
        vec3 worldPos = vec3(0.0), normal = vec3(0.0), diffuse = vec3(0.0);                                                                 \n \
        for (int i = 0; i < UnifPointLightPass.mNumSamples; i++)                                                                            \n \
        {                                                                                                                                   \n \
            worldPos += texelFetch(unifPositionTexture, texcoord, i).rgb;                                                                   \n \
            normal   += texelFetch(unifNormalTexture, texcoord, i).rgb;                                                                     \n \
            diffuse  += texelFetch(unifDiffuseTexture, texcoord, i).rgb;                                                                    \n \
        }                                                                                                                                   \n \
        worldPos /= UnifPointLightPass.mNumSamples;                                                                                         \n \
        normal   /= UnifPointLightPass.mNumSamples;                                                                                         \n \
        diffuse  /= UnifPointLightPass.mNumSamples;                                                                                         \n \
        normal = normalize(normal);                                                                                                         \n \
                                                                                                                                            \n \
        vec3 positionDiff = (UnifPointLightPass.mLightPos.xyz - worldPos);                                                                  \n \
        vec3 cubemapDir   = (worldPos - UnifPointLightPass.mLightPos.xyz);                                                                  \n \
                                                                                                                                            \n \
        float storedDepth = texture(unifShadowmap, cubemapDir);                                                                             \n \
        float visibility = 0.0;                                                                                                             \n \
        if (storedDepth + 0.0001 > VectorToDepthValue(cubemapDir))                                                                          \n \
            visibility = 1.0;                                                                                                               \n \
                                                                                                                                            \n \
        float dist = length(positionDiff);                                                                                                  \n \
        float attenuation = clamp(1.0 - dist*dist * (1 / (UnifPointLightPass.mMaxDistance * UnifPointLightPass.mMaxDistance)), 0.0, 1.0);   \n \
        attenuation *= attenuation;                                                                                                         \n \
                                                                                                                                            \n \
        vec3 lightDir = normalize(positionDiff);                                                                                            \n \
        float angleNormal = clamp(dot(normalize(normal), lightDir), 0, 1);                                                                  \n \
                                                                                                                                            \n \
        fragColor = vec4(diffuse, 1.0) * visibility * (angleNormal * attenuation *                                                          \n \
                    UnifPointLightPass.mLightIntensity * UnifPointLightPass.mLightColor);                                                   \n \
    }                                                                                                                                       \n";
}