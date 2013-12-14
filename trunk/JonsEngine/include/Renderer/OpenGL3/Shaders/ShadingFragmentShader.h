#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gShadingFragmentShader = 
    "#version 330                                                                                                           \n \
                                                                                                                            \n \
    layout(std140) uniform;                                                                                                 \n \
                                                                                                                            \n \
    uniform UnifShading                                                                                                     \n \
    {                                                                                                                       \n \
        mat4 mWVPMatrix;                                                                                                    \n \
        vec4 mLightColor;                                                                                                   \n \
        vec4 mLightPosOrDir;                                                                                                \n \
        vec4 mGamma;                                                                                                        \n \
        vec2 mScreenSize;                                                                                                   \n \
                                                                                                                            \n \
        int   mLightType;                                                                                                   \n \
        float mFalloffFactor;                                                                                               \n \
        float mMaxDistance;                                                                                                 \n \
    } UnifShadingPass;                                                                                                      \n \
                                                                                                                            \n \
    uniform sampler2D unifPositionTexture;                                                                                  \n \
    uniform sampler2D unifNormalTexture;                                                                                    \n \
    uniform sampler2D unifDiffuseTexture;                                                                                   \n \
                                                                                                                            \n \
    out vec4 fragColor;                                                                                                     \n \
                                                                                                                            \n \
    vec4 CalcPointLight(vec3 worldPos, vec3 normal)                                                                        \n \
    {                                                                                                                       \n \
       return UnifShadingPass.mLightColor;                                                                                  \n \
    }                                                                                                                       \n \
                                                                                                                            \n \
    vec4 CalcDirectionalLight(vec3 worldPos, vec3 normal)                                                                  \n \
    {                                                                                                                       \n \
        return UnifShadingPass.mLightColor;                                                                                 \n \
    }                                                                                                                       \n \
                                                                                                                            \n \
    vec4 CalcAmbientLight()                                                                                                 \n \
    {                                                                                                                       \n \
        return UnifShadingPass.mLightColor;                                                                                 \n \
    }                                                                                                                       \n \
                                                                                                                            \n \
    void main()                                                                                                             \n \
    {                                                                                                                       \n \
        vec2 texcoord = gl_FragCoord.xy / UnifShadingPass.mScreenSize;                                                      \n \
                                                                                                                            \n \
        vec3 worldPos = texture(unifPositionTexture, texcoord).xyz;                                                         \n \
        vec3 normal   = texture(unifNormalTexture, texcoord).xyz;                                                           \n \
        vec3 diffuse  = texture(unifDiffuseTexture, texcoord).xyz;                                                          \n \
        normal        = normalize(normal);                                                                                  \n \
                                                                                                                            \n \
        if (UnifShadingPass.mLightType == 1)                                                                                \n \
            fragColor = vec4(diffuse, 1.0) * CalcPointLight(worldPos, normal);                 \n \
        else if (UnifShadingPass.mLightType == 2)                                                                           \n \
            fragColor = vec4(diffuse, 1.0) * CalcDirectionalLight(worldPos, normal);           \n \
        else if (UnifShadingPass.mLightType == 3)                                                                           \n \
            fragColor = vec4(diffuse, 1.0) * CalcAmbientLight();                                            \n \
        else                                                                                                                \n \
            fragColor = vec4(diffuse, 0.0);                                                                                          \n \
    }                                                                                                                       \n";
}