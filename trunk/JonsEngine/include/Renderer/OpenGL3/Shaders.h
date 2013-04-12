#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								                            \n \
                                                                                                                \n \
                                        layout(std140) uniform UniTransform                                     \n \
                                        {                                                                       \n \
                                            vec4 Color;                                                         \n \
                                            mat4 WVPMatrix;                                                     \n \
                                            mat4 WorldMatrix;                                                   \n \
                                        } Transform;                                                            \n \
                                                                                                                \n \
                                        layout(location = 0) in vec3 vert_position;				                \n \
                                        layout(location = 1) in vec3 vert_normal;					            \n \
                                                                                                                \n \
                                        out vec4 frag_color;                                                    \n \
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
                                            frag_color    = Transform.Color;                                    \n \
                                            frag_position = position.xyz;                                       \n \
                                            frag_normal   = normal.xyz;                                         \n \
                                        }											                            \n";


    const std::string gFragmentShader =	"#version 330									                                                \n \
                                                                                                                                        \n \
                                        layout(std140) uniform UniLight                                                                 \n \
                                        {                                                                                               \n \
                                            vec4  AmbientLight;                                                                         \n \
                                            vec4  LightIntensity;                                                                       \n \
                                            vec4  Gamma;                                                                                 \n \
                                            vec3  LightPosition;                                                                        \n \
                                            vec3  ViewDirection;                                                                        \n \
                                            float LightAttenuation;                                                                     \n \
                                            float ShininessFactor;                                                                      \n \
                                        } Light;                                                                                        \n \
                                                                                                                                        \n \
                                        in vec4 frag_color;                                                                             \n \
                                        in vec3 frag_position;                                                                          \n \
                                        in vec3 frag_normal;                                                                            \n \
                                                                                                                                        \n \
                                        out vec4 finalColor;						                                                    \n \
                                                                                                                                        \n \
                                        void CalcLightIntensity(out vec3 positionDiff, out vec4 attenIntensity)                         \n \
                                        {                                                                                               \n \
                                            positionDiff   = Light.LightPosition - frag_position;                                       \n \
                                            float distance = length(positionDiff);                                                      \n \
                                            attenIntensity = Light.LightIntensity * (1 / (1.0 + (distance * Light.LightAttenuation)));  \n \
                                        }                                                                                               \n \
                                                                                                                                        \n \
                                        void CalcGaussianSpecular(in vec3 dirToLight, in vec4 specularColor, in float angIncidence,     \n \
                                                                    out float gaussianTerm)                                             \n \
                                        {                                                                                               \n \
                                            vec3 viewDirection = normalize(Light.ViewDirection);                                        \n \
                                            vec3 halfAngle     = normalize(dirToLight + viewDirection);                                 \n \
                                                                                                                                        \n \
                                            float angleNormalHalf = acos(dot(halfAngle, normalize(frag_normal)));                       \n \
                                            float exponent        = angleNormalHalf / Light.ShininessFactor;                            \n \
                                            exponent              = -(exponent * exponent);                                             \n \
                                                                                                                                        \n \
                                            gaussianTerm = exp(exponent);                                                               \n \
                                            gaussianTerm = angIncidence != 0.0 ? gaussianTerm : 0.0;                                    \n \
                                        }                                                                                               \n \
                                                                                                                                        \n \
                                        void main()										                                                \n \
                                        {												                                                \n \
                                            vec3 positionDiff   = vec3(0.0);                                                            \n \
                                            vec4 attenIntensity = vec4(0.0);                                                            \n \
                                            CalcLightIntensity(positionDiff, attenIntensity);                                           \n \
                                                                                                                                        \n \
                                            vec3 dirToLight    = normalize(positionDiff);                                               \n \
                                            float angIncidence = dot(normalize(frag_normal), dirToLight);                               \n \
                                            angIncidence       = clamp(angIncidence, 0, 1);                                             \n \
                                                                                                                                        \n \
                                            vec4 specularColor = vec4(0.4, 0.4, 0.4, 1.0);                                              \n \
                                            float gaussianTerm = 0.0;                                                                   \n \
                                            CalcGaussianSpecular(dirToLight, specularColor, angIncidence, gaussianTerm);                \n \
                                                                                                                                        \n \
                                            vec4 accumLighting = (frag_color * attenIntensity * angIncidence) +                         \n \
                                                                 (specularColor * attenIntensity * gaussianTerm) +                      \n \
                                                                 (frag_color * Light.AmbientLight);                                     \n \
                                                                                                                                        \n \
                                            finalColor = pow(accumLighting, Light.Gamma);                                               \n \
                                        }												                                                \n";
}