#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gVertexShader =
    "#version 330								                                            \n \
                                                                                            \n \
    layout(std140) uniform;                                                                 \n \
                                                                                            \n \
    uniform UnifMaterial                                                                    \n \
    {                                                                                       \n \
        vec4  mDiffuseColor;                                                                \n \
        vec4  mAmbientColor;                                                                \n \
        vec4  mSpecularColor;                                                               \n \
        vec4  mEmissiveColor;                                                               \n \
                                                                                            \n \
        bool  mHasDiffuseTexture;                                                           \n \
        bool  mHasNormalTexture;                                                            \n \
        bool  mLightingEnabled;                                                             \n \
        float mSpecularShininess;                                                           \n \
    } Material;                                                                             \n \
                                                                                            \n \
    uniform UnifTransform                                                                   \n \
    {                                                                                       \n \
        mat4 mWVPMatrix;                                                                    \n \
        mat4 mWorldMatrix;                                                                  \n \
        float mTextureTilingFactor;                                                         \n \
    } Transform;                                                                            \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;				                                \n \
    layout(location = 1) in vec3 vert_normal;					                            \n \
    layout(location = 2) in vec2 vert_texcoord;					                            \n \
    layout(location = 3) in vec3 vert_tangent;                                              \n \
    layout(location = 4) in vec3 vert_bitangent;                                            \n \
                                                                                            \n \
    out vec3 frag_position;                                                                 \n \
    out vec3 frag_normal;                                                                   \n \
    out vec2 frag_texcoord;                                                                 \n \
    out vec3 frag_tangent;                                                                  \n \
    out vec3 frag_bitangent;                                                                \n \
                                                                                            \n \
    void main()									                                            \n \
    {											                                            \n \
        gl_Position   = Transform.mWVPMatrix * vec4(vert_position, 1.0);                    \n \
                                                                                            \n \
        frag_position = (Transform.mWorldMatrix * vec4(vert_position, 1.0)).xyz;            \n \
        frag_normal   = (Transform.mWorldMatrix * vec4(vert_normal, 0.0)).xyz;              \n \
                                                                                            \n \
        if (Material.mHasDiffuseTexture)                                                    \n \
            frag_texcoord = Transform.mTextureTilingFactor * vert_texcoord;                 \n \
                                                                                            \n \
        if (Material.mHasNormalTexture)                                                     \n \
        {                                                                                   \n \
            frag_tangent   = (Transform.mWorldMatrix * vec4(frag_tangent, 0.0)).xyz;        \n \
            frag_bitangent = (Transform.mWorldMatrix * vec4(frag_bitangent, 0.0)).xyz;      \n \
        }                                                                                   \n \
    }											                                            \n";


    const std::string gFragmentShader =	
    "#version 330									                                                                    \n \
                                                                                                                        \n \
    layout(std140) uniform;                                                                                             \n \
                                                                                                                        \n \
    const int MAX_LIGHTS = 8;                                                                                           \n \
                                                                                                                        \n \
    struct Light                                                                                                        \n \
    {                                                                                                                   \n \
        vec4  mLightColor;                                                                                              \n \
        vec4  mLightPosition;                                                                                           \n \
        vec4  mLightDirection;                                                                                          \n \
                                                                                                                        \n \
        int   mLightType;                                                                                               \n \
        float mLightIntensity;                                                                                          \n \
        float mLightRadius;                                                                                             \n \
        float mMaxDistance;                                                                                             \n \
    };                                                                                                                  \n \
                                                                                                                        \n \
    uniform UnifLighting                                                                                                \n \
    {                                                                                                                   \n \
        vec4  mGamma;                                                                                                   \n \
        vec3  mViewDirection;                                                                                           \n \
        int   mNumLights;                                                                                               \n \
                                                                                                                        \n \
        Light mLights[MAX_LIGHTS];                                                                                      \n \
    } Lighting;                                                                                                         \n \
                                                                                                                        \n \
    uniform UnifMaterial                                                                                                \n \
    {                                                                                                                   \n \
        vec4  mDiffuseColor;                                                                                            \n \
        vec4  mAmbientColor;                                                                                            \n \
        vec4  mSpecularColor;                                                                                           \n \
        vec4  mEmissiveColor;                                                                                           \n \
                                                                                                                        \n \
        bool  mHasDiffuseTexture;                                                                                       \n \
        bool  mHasNormalTexture;                                                                                        \n \
        bool  mLightingEnabled;                                                                                         \n \
        float mSpecularShininess;                                                                                       \n \
    } Material;                                                                                                         \n \
                                                                                                                        \n \
    uniform sampler2D unifDiffuseTexture;                                                                               \n \
    uniform sampler2D unifNormalTexture;                                                                                \n \
                                                                                                                        \n \
    in vec3 frag_position;                                                                                              \n \
    in vec3 frag_normal;                                                                                                \n \
    in vec2 frag_texcoord;                                                                                              \n \
    in vec3 frag_tangent;                                                                                               \n \
    in vec3 frag_bitangent;                                                                                             \n \
                                                                                                                        \n \
    out vec4 finalColor;						                                                              "  
                                                                                                              "         \n \
                                                                                                                        \n \
    void CalcGaussianSpecular(in vec3 dirToLight, in vec3 normal, out float gaussianTerm)                                               \n \
    {                                                                                                                   \n \
        vec3 viewDirection = normalize(Lighting.mViewDirection);                                                        \n \
        vec3 halfAngle     = normalize(dirToLight + viewDirection);                                                     \n \
                                                                                                                        \n \
        float angleNormalHalf = acos(dot(halfAngle, normalize(normal)));                                           \n \
        float exponent        = angleNormalHalf / Material.mSpecularShininess;                                          \n \
        exponent              = -(exponent * exponent);                                                                 \n \
                                                                                                                        \n \
        gaussianTerm = exp(exponent);                                                                                   \n \
    }                                                                                                                   \n \
                                                                                                                        \n \
    vec4 CalculateLighting(in Light light, in vec4 diffuseTexture, in vec3 normal)                                                      \n \
    {                                                                                                                   \n \
        if (light.mLightType == 1)          // point light                                                              \n \
        {                                                                                                               \n \
            vec3 positionDiff = light.mLightPosition.xyz - frag_position;                                                   \n \
            float dist        = max(length(positionDiff) - light.mLightRadius, 0);                                          \n \
                                                                                                                            \n \
            float attenuation = 1 / ((dist/light.mLightRadius + 1) * (dist/light.mLightRadius + 1));                        \n \
            attenuation       = max((attenuation - light.mMaxDistance) / (1 - light.mMaxDistance), 0);                      \n \
                                                                                                                            \n \
            vec3 dirToLight   = normalize(positionDiff);                                                                    \n \
            float angleNormal = clamp(dot(normalize(normal), dirToLight), 0, 1);                                       \n \
                                                                                                                            \n \
            float gaussianTerm = 0.0;                                                                                                         \n \
            if (angleNormal > 0.0)                                                                                                            \n \
                CalcGaussianSpecular(dirToLight, normal, gaussianTerm);                                                                               \n \
                                                                                                                                                \n \
            return diffuseTexture * (attenuation * angleNormal  * Material.mDiffuseColor  * light.mLightIntensity * light.mLightColor) +      \n \
                                    (attenuation * gaussianTerm * Material.mSpecularColor * light.mLightIntensity * light.mLightColor);       \n \
        }                                                                                                                                     \n \
        else if (light.mLightType == 2)     // directional light                                                                              \n \
        {                                                                                                                                     \n \
            vec3 dirToLight   = normalize(light.mLightDirection.xyz);                                                                         \n \
            float angleNormal = clamp(dot(normalize(normal), dirToLight), 0, 1);                                                         \n \
                                                                                                                            \n \
            float gaussianTerm = 0.0;                                                                                       \n \
            if (angleNormal > 0.0)                                                                                          \n \
                CalcGaussianSpecular(dirToLight, normal, gaussianTerm);                                                             \n \
                                                                                                                            \n \
            return diffuseTexture * (angleNormal  * Material.mDiffuseColor  * light.mLightIntensity * light.mLightColor) +  \n \
                                    (gaussianTerm * Material.mSpecularColor * light.mLightIntensity * light.mLightColor);   \n \
        }                                                                                                                   \n \
        else if (light.mLightType == 4)     // ambient light                                                            \n \
            return diffuseTexture * Material.mAmbientColor * light.mLightIntensity * light.mLightColor;                 \n \
        else                                                                                                            \n \
            return vec4(0.0);                                                                                           \n \
    }                                                                                                                   \n \
                                                                                                                        \n \
    void main()										                                                                    \n \
    {												                                                                    \n \
        vec4 diffuseTexture = vec4(1.0);                                                                                \n \
        if (Material.mHasDiffuseTexture)                                                                                \n \
            diffuseTexture = texture(unifDiffuseTexture, frag_texcoord);                                                \n \
                                                                                                                        \n \
        vec3 normal = frag_normal;                                                                                        \n \
        if (Material.mHasNormalTexture)                                                                                 \n \
        {                                                                                                               \n \
           // diffuseTexture = vec4(normalize(texture(unifNormalTexture, frag_texcoord).xyz * 2.0 - 1.0), 1.0);  \n \
           // vec3 normalTangentSpace  = normalize(texture(unifNormalTexture, frag_texcoord).xyz * 2.0 - 1.0);            \n \
            //mat3 tangentToWorldSpace = mat3(normalize(frag_tangent), normalize(frag_bitangent), normalize(frag_normal)); \n \
                                                                                                                            \n \
           // normal = tangentToWorldSpace * normalTangentSpace;                                                              \n \
        }                                                                                                                   \n \
                                                                                                                        \n \
        if (Material.mLightingEnabled)                                                                                  \n \
        {                                                                                                               \n \
            vec4 accumLighting = vec4(0.0);                                                                             \n \
                                                                                                                        \n \
            for (int lightIndex = 0; lightIndex < Lighting.mNumLights; lightIndex++)                                    \n \
                accumLighting += Material.mEmissiveColor * diffuseTexture +                                             \n \
                                    CalculateLighting(Lighting.mLights[lightIndex], diffuseTexture, normal);                       \n \
                                                                                                                        \n \
            finalColor = pow(accumLighting, Lighting.mGamma);                                                           \n \
        }                                                                                                               \n \
        else {                                                                                                          \n \
            finalColor = pow(diffuseTexture, Lighting.mGamma);                                                          \n \
        }                                                                                                               \n \
    }												                                                                    \n";
}