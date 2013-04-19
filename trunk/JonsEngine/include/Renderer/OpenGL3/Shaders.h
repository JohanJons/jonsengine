#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								                            \n \
                                                                                                                \n \
                                        layout(std140) uniform;                                                 \n \
                                                                                                                \n \
                                        uniform UniTransform                                                    \n \
                                        {                                                                       \n \
                                            mat4 WVPMatrix;                                                     \n \
                                            mat4 WorldMatrix;                                                   \n \
                                        } Transform;                                                            \n \
                                                                                                                \n \
                                        layout(location = 0) in vec3 vert_position;				                \n \
                                        layout(location = 1) in vec3 vert_normal;					            \n \
                                                                                                                \n \
                                        out vec3 frag_position;                                                 \n \
                                        out vec3 frag_normal;                                                   \n \
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
                                        }											                            \n";


    const std::string gFragmentShader =	"#version 330									                                                            \n \
                                                                                                                                                    \n \
                                        layout(std140) uniform;                                                                                     \n \
                                                                                                                                                    \n \
                                        const int MAX_LIGHTS = 8;                                                                                   \n \
                                                                                                                                                    \n \
                                        uniform UniLightingInfo                                                                                     \n \
                                        {                                                                                                           \n \
                                            vec4  mAmbientLight;                                                                                    \n \
                                            vec4  mGamma;                                                                                           \n \
                                            vec3  mViewDirection;                                                                                   \n \
                                            int   mNumLights;                                                                                       \n \
                                        } LightingInfo;                                                                                             \n \
                                                                                                                                                    \n \
                                        struct Light                                                                                                \n \
                                        {                                                                                                           \n \
                                            vec4  mLightIntensity;                                                                                  \n \
                                            vec4  mLightPosition;                                                                                   \n \
                                            float mLightAttenuation;                                                                                \n \
                                            float mMaxAttenuation;                                                                                  \n \
                                        };                                                                                                          \n \
                                                                                                                                                    \n \
                                        uniform UniLights                                                                                           \n \
                                        {                                                                                                           \n \
                                            Light mLights[MAX_LIGHTS];                                                                              \n \
                                        } Lights;                                                                                                   \n \
                                                                                                                                                    \n \
                                        uniform UniMaterial                                                                                         \n \
                                        {                                                                                                           \n \
                                            vec4  mDiffuseColor;                                                                                    \n \
                                            float mSpecularFactor;                                                                                  \n \
                                        } Material;                                                                                                 \n \
                                                                                                                                                    \n \
                                        in vec3 frag_position;                                                                                      \n \
                                        in vec3 frag_normal;                                                                                        \n \
                                                                                                                                                    \n \
                                        out vec4 finalColor;						                                                                \n \
                                                                                                                                                    \n \
                                                                                                                                                    \n \
                                        void CalcLightIntensity(in Light light, out vec3 positionDiff, out vec4 attenIntensity)                     \n \
                                        {                                                                                                           \n \
                                            positionDiff      = light.mLightPosition.xyz - frag_position;                                           \n \
                                            float distance    = length(positionDiff);                                                               \n \
                                            float attenuation = 1 / (1.0 + (light.mLightAttenuation * distance * distance));                        \n \
                                            attenIntensity    = light.mLightIntensity * (attenuation < light.mMaxAttenuation ? 0.0 : attenuation);  \n \
                                        }                                                                                                           \n \
                                                                                                                                                    \n \
                                        void CalcGaussianSpecular(in vec3 dirToLight, in vec4 specularColor, in float angIncidence,                 \n \
                                                                    out float gaussianTerm)                                                         \n \
                                        {                                                                                                           \n \
                                            vec3 viewDirection = normalize(LightingInfo.mViewDirection);                                            \n \
                                            vec3 halfAngle     = normalize(dirToLight + viewDirection);                                             \n \
                                                                                                                                                    \n \
                                            float angleNormalHalf = acos(dot(halfAngle, normalize(frag_normal)));                                   \n \
                                            float exponent        = angleNormalHalf / Material.mSpecularFactor;                                     \n \
                                            exponent              = -(exponent * exponent);                                                         \n \
                                                                                                                                                    \n \
                                            gaussianTerm = exp(exponent);                                                                           \n \
                                            gaussianTerm = angIncidence != 0.0 ? gaussianTerm : 0.0;                                                \n \
                                        }                                                                                                           \n \
                                                                                                                                                    \n \
                                        vec4 CalculateLighting(in Light light)                                                                      \n \
                                        {                                                                                                           \n \
                                            vec3 positionDiff   = vec3(0.0);                                                                        \n \
                                            vec4 attenIntensity = vec4(0.0);                                                                        \n \
                                            if (light.mLightPosition.w != 0.0)  // w == 0 means directional light                                   \n \
                                                CalcLightIntensity(light, positionDiff, attenIntensity);                                            \n \
                                            else                                                                                                    \n \
                                            {                                                                                                       \n \
                                                positionDiff   = vec3(light.mLightPosition);                                                        \n \
                                                attenIntensity = light.mLightIntensity;                                                             \n \
                                            }                                                                                                       \n \
                                                                                                                                                    \n \
                                            vec3 dirToLight    = normalize(positionDiff);                                                           \n \
                                            float angIncidence = dot(normalize(frag_normal), dirToLight);                                           \n \
                                            angIncidence       = clamp(angIncidence, 0, 1);                                                         \n \
                                                                                                                                                    \n \
                                            vec4 specularColor = vec4(0.4, 0.4, 0.4, 1.0);                                                          \n \
                                            float gaussianTerm = 0.0;                                                                               \n \
                                            CalcGaussianSpecular(dirToLight, specularColor, angIncidence, gaussianTerm);                            \n \
                                                                                                                                                    \n \
                                            vec4 lighting = (Material.mDiffuseColor * attenIntensity * angIncidence) +                              \n \
                                                            (specularColor * attenIntensity * gaussianTerm);                                        \n \
                                                                                                                                                    \n \
                                            return lighting;                                                                                        \n \
                                        }                                                                                                           \n \
                                                                                                                                                    \n \
                                        void main()										                                                            \n \
                                        {												                                                            \n \
                                            vec4 accumLighting = Material.mDiffuseColor * LightingInfo.mAmbientLight;                               \n \
                                            for (int lightIndex = 0; lightIndex < LightingInfo.mNumLights; lightIndex++)                            \n \
                                                accumLighting += CalculateLighting(Lights.mLights[lightIndex]);                                     \n \
                                                                                                                                                    \n \
                                            finalColor = pow(accumLighting, LightingInfo.mGamma);                                                   \n \
                                        }												                                                            \n";
}