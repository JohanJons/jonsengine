#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gShadingFragmentShader = 
    "#version 430                                                                                                           \n \
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
    layout (binding = 2) uniform sampler2D unifPositionTexture;                                                             \n \
    layout (binding = 3) uniform sampler2D unifNormalTexture;                                                               \n \
    layout (binding = 4) uniform sampler2D unifDiffuseTexture;                                                              \n \
                                                                                                                            \n \
    out vec4 fragColor;                                                                                                     \n \
                                                                                                                            \n \
    vec4 CalcPointLight(vec3 worldPos, vec3 normal)                                                                         \n \
    {                                                                                                                       \n \
       vec3 positionDiff = (UnifShadingPass.mLightPosOrDir.xyz - worldPos);                                                 \n \
                                                                                                                            \n \
       float dist = length(positionDiff);                                                                                   \n \
       float attenuation = clamp(1.0 - (dist / UnifShadingPass.mMaxDistance), 0.0, 1.0);                                    \n \
                                                                                                                            \n \
       vec3 lightDir = normalize(positionDiff);                                                                             \n \
       float angleNormal = clamp(dot(normalize(normal), lightDir), 0, 1);                                                   \n \
                                                                                                                            \n \
       return angleNormal * attenuation * UnifShadingPass.mLightColor;                                                      \n \
    }                                                                                                                       \n \
                                                                                                                            \n \
    vec4 CalcDirectionalLight(vec3 worldPos, vec3 normal)                                                                  \n \
    {                                                                                                                       \n \
        float angleNormal = clamp(dot(normalize(normal), UnifShadingPass.mLightPosOrDir.xyz), 0, 1);                        \n \
                                                                                                                            \n \
        return angleNormal * UnifShadingPass.mLightColor;                                                                   \n \
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
            fragColor = vec4(diffuse, 1.0) * CalcPointLight(worldPos, normal);                                              \n \
        else if (UnifShadingPass.mLightType == 2)                                                                           \n \
            fragColor = vec4(diffuse, 1.0) * CalcDirectionalLight(worldPos, normal);                                        \n \
        else if (UnifShadingPass.mLightType == 3)                                                                           \n \
            fragColor = vec4(diffuse, 1.0) * CalcAmbientLight();                                                            \n \
        else                                                                                                                \n \
            fragColor = vec4(diffuse, 0.0);                                                                                 \n \
    }                                                                                                                       \n";


    /*
      vec3 distSquared = dist * dist; \n \
       float attS = 15.0; \n \
       float attenuation = distSquared / (UnifShadingPass.mMaxDistance * UnifShadingPass.mMaxDistance);                     \n \
       attenuation = 1.0 / (attenuation * attS + 1.0); \n \
        attS = 1.0 / (attS + 1.0); \n \
         attenuation = attenuation - attS; \n \
         attenuation = attenuation / (1.0 - attS); \n \
    
    */
}