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
    } UnifPointLightPass;                                                                                                                   \n \
                                                                                                                                            \n \
    layout (binding = 2) uniform sampler2D unifPositionTexture;                                                                             \n \
    layout (binding = 3) uniform sampler2D unifNormalTexture;                                                                               \n \
    layout (binding = 4) uniform sampler2D unifDiffuseTexture;                                                                              \n \
    layout (binding = 7) uniform samplerCube unifShadowmap;                                                                           \n \
                                                                                                                                            \n \
    out vec4 fragColor;                                                                                                                     \n \
                                                                                                                                            \n \
    float VectorToDepthValue(vec3 Vec) \n \
    { \n \
        vec3 AbsVec = abs(Vec); \n \
        float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z)); \n \
        \n \
        const float f = 100.0; \n \
        const float n = 0.1; \n \
        float NormZComp = (f + n) / (f - n) - (2 * f*n) / (f - n) / LocalZcomp; \n \
        return (NormZComp + 1.0) * 0.5; \n \
    } \n \
    \n \
    void main()                                                                                                                             \n \
    {                                                                                                                                       \n \
        vec2 texcoord = gl_FragCoord.xy / UnifPointLightPass.mScreenSize;                                                                   \n \
                                                                                                                                            \n \
        vec3 worldPos = texture(unifPositionTexture, texcoord).xyz;                                                                         \n \
        vec3 normal   = texture(unifNormalTexture, texcoord).xyz;                                                                           \n \
        vec3 diffuse  = texture(unifDiffuseTexture, texcoord).xyz;                                                                          \n \
        normal        = normalize(normal);                                                                                                  \n \
                                                                                                                                            \n \
        vec3 positionDiff = (UnifPointLightPass.mLightPos.xyz - worldPos);                                                                   \n \
                                                                                                                                            \n \
        float storedDepth = texture(unifShadowmap, positionDiff);                                                                            \n \
        float visibility = 0.0; \n \
        if (storedDepth + 0.0001 > VectorToDepthValue(positionDiff)) \n \
               visibility = 1.0;                                                                                                                             \n \
        float dist = length(positionDiff);                                                                                                   \n \
        float attenuation = clamp(1.0 - dist*dist * (1 / (UnifPointLightPass.mMaxDistance * UnifPointLightPass.mMaxDistance)), 0.0, 1.0);    \n \
        attenuation *= attenuation;                                                                                                          \n \
                                                                                                                                            \n \
        vec3 lightDir = normalize(positionDiff);                                                                                             \n \
        float angleNormal = clamp(dot(normalize(normal), lightDir), 0, 1);                                                                   \n \
                                                                                                                                            \n \
        fragColor = vec4(diffuse, 1.0) * visibility * (angleNormal * attenuation * UnifPointLightPass.mLightIntensity * UnifPointLightPass.mLightColor);  \n \
    }                                                                                                                                       \n";
}