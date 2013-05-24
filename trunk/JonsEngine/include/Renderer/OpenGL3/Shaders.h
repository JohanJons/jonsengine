#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								                            \n \
                                                                                                                \n \
                                        layout(std140) uniform;                                                 \n \
                                                                                                                \n \
                                        uniform UnifTransform                                                   \n \
                                        {                                                                       \n \
                                            mat4 WVPMatrix;                                                     \n \
                                            mat4 WorldMatrix;                                                   \n \
                                        } Transform;                                                            \n \
                                                                                                                \n \
                                        layout(location = 0) in vec3 vert_position;				                \n \
                                        layout(location = 1) in vec3 vert_normal;					            \n \
                                        layout(location = 2) in vec2 vert_texcoord;					            \n \
                                                                                                                \n \
                                        out vec3 frag_position;                                                 \n \
                                        out vec3 frag_normal;                                                   \n \
                                        out vec2 frag_texcoord;                                                 \n \
                                                                                                                \n \
                                        void main()									                            \n \
                                        {											                            \n \
                                            gl_Position   = Transform.WVPMatrix * vec4(vert_position, 1.0);     \n \
                                                                                                                \n \
                                            vec4 position = Transform.WorldMatrix * vec4(vert_position, 1.0);   \n \
                                            vec4 normal   = Transform.WorldMatrix * vec4(vert_normal, 0.0);     \n \
                                                                                                                \n \
                                            frag_position = position.xyz;                                       \n \
                                            frag_normal   = normal.xyz;                                         \n \
                                            frag_texcoord = vert_texcoord;                                      \n \
                                        }											                            \n";


    const std::string gFragmentShader =	"#version 330									                                                                    \n \
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
                                            int   mHasDiffuseTexture;                                                                                       \n \
                                            float mSpecularShininess;                                                                                       \n \
                                        } Material;                                                                                                         \n \
                                                                                                                                                            \n \
                                        uniform sampler2D unifDiffuseTexture;                                                                               \n \
                                                                                                                                                            \n \
                                        in vec3 frag_position;                                                                                              \n \
                                        in vec3 frag_normal;                                                                                                \n \
                                        in vec2 frag_texcoord;                                                                                              \n \
                                                                                                                                                            \n \
                                        out vec4 finalColor;						                                                              "  
                                                                                                                                                  "         \n \
                                                                                                                                                            \n \
                                                                                                                                                            \n \
                                        void CalcGaussianSpecular(in vec3 dirToLight, out float gaussianTerm)                                               \n \
                                        {                                                                                                                   \n \
                                            vec3 viewDirection = normalize(Lighting.mViewDirection);                                                        \n \
                                            vec3 halfAngle     = normalize(dirToLight + viewDirection);                                                     \n \
                                                                                                                                                            \n \
                                            float angleNormalHalf = acos(dot(halfAngle, normalize(frag_normal)));                                           \n \
                                            float exponent        = angleNormalHalf / Material.mSpecularShininess;                                          \n \
                                            exponent              = -(exponent * exponent);                                                                 \n \
                                                                                                                                                            \n \
                                            gaussianTerm = exp(exponent);                                                                                   \n \
                                        }                                                                                                                   \n \
                                                                                                                                                            \n \
                                        vec4 CalcPointLight(in Light light)                                                                                 \n \
                                        {                                                                                                                   \n \
                                            vec3 positionDiff = light.mLightPosition.xyz - frag_position;                                                   \n \
                                            float dist        = length(positionDiff);                                                                       \n \
                                            dist              = max(dist - light.mLightRadius, 0);                                                          \n \
                                                                                                                                                            \n \
                                            float attenuation = 1 / ((dist/light.mLightRadius + 1) * (dist/light.mLightRadius + 1));                        \n \
                                            attenuation       = (attenuation - light.mMaxDistance) / (1 - light.mMaxDistance);                              \n \
                                            attenuation       = max(attenuation, 0);                                                                        \n \
                                                                                                                                                            \n \
                                            vec3 dirToLight   = normalize(positionDiff);                                                                    \n \
                                            float angleNormal = dot(normalize(frag_normal), dirToLight);                                                    \n \
                                            angleNormal       = clamp(angleNormal, 0, 1);                                                                   \n \
                                                                                                                                                            \n \
                                            float gaussianTerm = 0.0;                                                                                       \n \
                                            if (angleNormal > 0.0)                                                                                          \n \
                                                CalcGaussianSpecular(dirToLight, gaussianTerm);                                                             \n \
                                                                                                                                                            \n \
                                           return (attenuation * angleNormal  * Material.mDiffuseColor  * light.mLightIntensity * light.mLightColor) +      \n \
                                                  (attenuation * gaussianTerm * Material.mSpecularColor * light.mLightIntensity * light.mLightColor);       \n \
                                        }                                                                                                                   \n \
                                                                                                                                                            \n \
                                        vec4 CalcDirectionalLight(in Light light)                                                                           \n \
                                        {                                                                                                                   \n \
                                            vec3 dirToLight   = normalize(light.mLightDirection.xyz);                                                       \n \
                                            float angleNormal = dot(normalize(frag_normal), dirToLight);                                                    \n \
                                            angleNormal       = clamp(angleNormal, 0, 1);                                                                   \n \
                                                                                                                                                            \n \
                                            float gaussianTerm = 0.0;                                                                                       \n \
                                            if (angleNormal > 0.0)                                                                                          \n \
                                                CalcGaussianSpecular(dirToLight, gaussianTerm);                                                             \n \
                                                                                                                                                            \n \
                                            return (angleNormal  * Material.mDiffuseColor  * light.mLightIntensity * light.mLightColor) +                   \n \
                                                   (gaussianTerm * Material.mSpecularColor * light.mLightIntensity * light.mLightColor);                    \n \
                                        }                                                                                                                   \n \
                                                                                                                                                            \n \
                                        vec4 CalcAmbientLight(in Light light)                                                                               \n \
                                        {                                                                                                                   \n \
                                            return (Material.mAmbientColor * light.mLightIntensity * light.mLightColor);                                    \n \
                                        }                                                                                                                   \n \
                                                                                                                                                            \n \
                                        vec4 CalculateLighting(in Light light, in vec4 diffuseTexture)                                                      \n \
                                        {                                                                                                                   \n \
                                            if (light.mLightType == 1)          // point light                                                              \n \
                                                return CalcPointLight(light);                                                                               \n \
                                            else if (light.mLightType == 2)     // directional light                                                        \n \
                                                return CalcDirectionalLight(light);                                                                         \n \
                                            else if (light.mLightType == 4)     // ambient light                                                            \n \
                                                return CalcAmbientLight(light);                                                                             \n \
                                            else                                                                                                            \n \
                                                return vec4(0.0);                                                                                           \n \
                                        }                                                                                                                   \n \
                                                                                                                                                            \n \
                                        void main()										                                                                    \n \
                                        {												                                                                    \n \
                                            vec4 diffuseTexture = vec4(1.0);                                                                                \n \
                                            if (Material.mHasDiffuseTexture == 1)                                                                           \n \
                                                diffuseTexture = texture(unifDiffuseTexture, frag_texcoord);                                                \n \
                                                                                                                                                            \n \
                                            vec4 accumLighting = vec4(0.0); //Material.mEmissiveColor;                                                      \n \
                                                                                                                                                            \n \
                                            for (int lightIndex = 0; lightIndex < Lighting.mNumLights; lightIndex++)                                        \n \
                                                accumLighting += CalculateLighting(Lighting.mLights[lightIndex],diffuseTexture);                            \n \
                                                                                                                                                            \n \
                                            accumLighting = diffuseTexture * accumLighting;                                                                 \n \
                                                                                                                                                            \n \
                                            finalColor = pow(accumLighting, Lighting.mGamma);                                                               \n \
                                        }												                                                                    \n";
}