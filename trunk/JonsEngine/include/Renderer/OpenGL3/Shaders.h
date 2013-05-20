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


    const std::string gFragmentShader =	"#version 330									                                                            \n \
                                                                                                                                                    \n \
                                        layout(std140) uniform;                                                                                     \n \
                                                                                                                                                    \n \
                                        const int MAX_LIGHTS = 8;                                                                                   \n \
                                                                                                                                                    \n \
                                        struct Light                                                                                                \n \
                                        {                                                                                                           \n \
                                            vec4  mDiffuseColor;                                                                                    \n \
                                            vec4  mAmbientColor;                                                                                    \n \
                                            vec4  mSpecularColor;                                                                                   \n \
                                                                                                                                                    \n \
                                            vec4  mLightPosition;                                                                                   \n \
                                            vec4  mLightDirection;                                                                                  \n \
                                            int   mLightType;                                                                                       \n \
                                                                                                                                                    \n \
                                            float mConstantAttenutation;                                                                            \n \
                                            float mLinearAttenuation;                                                                               \n \
                                            float mQuadraticAttenuation;                                                                            \n \
                                            float mMaxDistance;                                                                                     \n \
                                        };                                                                                                          \n \
                                                                                                                                                    \n \
                                        uniform UnifLighting                                                                                        \n \
                                        {                                                                                                           \n \
                                            vec4  mGamma;                                                                                           \n \
                                            vec3  mViewDirection;                                                                                   \n \
                                            int   mNumLights;                                                                                       \n \
                                                                                                                                                    \n \
                                            Light mLights[MAX_LIGHTS];                                                                              \n \
                                        } Lighting;                                                                                                 \n \
                                                                                                                                                    \n \
                                        uniform UnifMaterial                                                                                        \n \
                                        {                                                                                                           \n \
                                            vec4  mDiffuseColor;                                                                                    \n \
                                            vec4  mAmbientColor;                                                                                    \n \
                                            vec4  mSpecularColor;                                                                                   \n \
                                            vec4  mEmissiveColor;                                                                                   \n \
                                                                                                                                                    \n \
                                            int   mHasDiffuseTexture;                                                                               \n \
                                            float mSpecularShininess;                                                                               \n \
                                        } Material;                                                                                                 \n \
                                                                                                                                                    \n \
                                        uniform sampler2D unifDiffuseTexture;                                                                       \n \
                                                                                                                                                    \n \
                                        in vec3 frag_position;                                                                                      \n \
                                        in vec3 frag_normal;                                                                                        \n \
                                        in vec2 frag_texcoord;                                                                                      \n \
                                                                                                                                                    \n \
                                        out vec4 finalColor;						                                                              "  
                                                                                                                                                  " \n \
                                                                                                                                                    \n \
                                                                                                                                                    \n \
                                        void CalcLightIntensityAndLightDirection(in Light light, out float attenuation, out vec3 dirToLight)        \n \
                                        {                                                                                                           \n \
                                            if (light.mLightType == 0)      // 0 == point light                                                     \n \
                                            {                                                                                                       \n \
                                                vec3 positionDiff = light.mLightPosition.xyz - frag_position;                                       \n \
                                                float dist        = length(positionDiff);                                                           \n \
                                                                                                                                                    \n \
                                                dirToLight = normalize(positionDiff);                                                               \n \
                                                                                                                                                    \n \
                                                if (dist <= light.mMaxDistance)                                                                     \n \
                                                    attenuation = 1 / (1.0 + (light.mConstantAttenutation + light.mLinearAttenuation * dist  +      \n \
                                                                              light.mQuadraticAttenuation * dist * dist));                          \n \
                                                else                                                                                                \n \
                                                    attenuation = 0.0;                                                                              \n \
                                            }                                                                                                       \n \
                                            else if (light.mLightType == 1)     // 1 == directional light                                           \n \
                                            {                                                                                                       \n \
                                                dirToLight = normalize(light.mLightDirection.xyz);                                                  \n \
                                            }                                                                                                       \n \
                                        }                                                                                                           \n \
                                                                                                                                                    \n \
                                        void CalcGaussianSpecular(in vec3 dirToLight, in float intensity, out float gaussianTerm)                   \n \
                                        {                                                                                                           \n \
                                            if (intensity > 0.0)                                                                                    \n \
                                            {                                                                                                       \n \
                                                vec3 viewDirection = normalize(Lighting.mViewDirection);                                            \n \
                                                vec3 halfAngle     = normalize(dirToLight + viewDirection);                                         \n \
                                                                                                                                                    \n \
                                                float angleNormalHalf = acos(dot(halfAngle, normalize(frag_normal)));                               \n \
                                                float exponent        = angleNormalHalf / Material.mSpecularShininess;                              \n \
                                                exponent              = -(exponent * exponent);                                                     \n \
                                                                                                                                                    \n \
                                                gaussianTerm = exp(exponent);                                                                       \n \
                                            }                                                                                                       \n \
                                            else                                                                                                    \n \
                                                gaussianTerm = 0.0;                                                                                 \n \
                                        }                                                                                                           \n \
                                                                                                                                                    \n \
                                        vec4 CalculateLighting(in Light light, in vec4 diffuseTexture)                                              \n \
                                        {                                                                                                           \n \
                                            vec3 dirToLight   = vec3(0.0);                                                                          \n \
                                            float attenuation = 1.0;                                                                                \n \
                                                                                                                                                    \n \
                                            CalcLightIntensityAndLightDirection(light, attenuation, dirToLight);                                    \n \
                                                                                                                                                    \n \
                                            float intensity = dot(normalize(frag_normal), dirToLight);                                              \n \
                                            intensity       = clamp(intensity, 0, 1);                                                               \n \
                                                                                                                                                    \n \
                                            float gaussianTerm = 0.0;                                                                               \n \
                                            if (intensity > 0.0)                                                                                    \n \
                                                CalcGaussianSpecular(dirToLight, intensity, gaussianTerm);                                          \n \
                                                                                                                                                    \n \
                                            return (attenuation * intensity    * Material.mDiffuseColor  * light.mDiffuseColor) +                   \n \
                                                   (attenuation * gaussianTerm * Material.mSpecularColor * light.mSpecularColor) +                  \n \
                                                   (Material.mAmbientColor  * light.mAmbientColor);                                                 \n \
                                        }                                                                                                           \n \
                                                                                                                                                    \n \
                                        void main()										                                                            \n \
                                        {												                                                            \n \
                                            vec4 diffuseTexture = vec4(1.0);                                                                        \n \
                                            if (Material.mHasDiffuseTexture == 1)                                                                   \n \
                                                diffuseTexture = texture(unifDiffuseTexture, frag_texcoord);                                        \n \
                                                                                                                                                    \n \
                                            vec4 accumLighting = vec4(0.0); //Material.mEmissiveColor;                                              \n \
                                                                                                                                                    \n \
                                            for (int lightIndex = 0; lightIndex < Lighting.mNumLights; lightIndex++)                                \n \
                                                accumLighting += CalculateLighting(Lighting.mLights[lightIndex],diffuseTexture);                    \n \
                                                                                                                                                    \n \
                                            accumLighting = diffuseTexture * accumLighting;                                                         \n \
                                                                                                                                                    \n \
                                            finalColor = pow(accumLighting, Lighting.mGamma);                                                       \n \
                                        }												                                                            \n";
}