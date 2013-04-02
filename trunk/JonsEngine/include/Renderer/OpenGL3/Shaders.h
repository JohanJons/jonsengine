#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								                            \n \
                                                                                                                \n \
                                        layout(std140) uniform Transform                                        \n \
                                        {                                                                       \n \
                                            vec4 Color;                                                         \n \
                                            mat4 WVPMatrix;                                                     \n \
                                            mat4 WorldMatrix;                                                   \n \
                                        };                                                                      \n \
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
                                            gl_Position = WVPMatrix * vec4(vert_position, 1.0);                 \n \
                                                                                                                \n \
                                            vec4 position = WorldMatrix * vec4(vert_position, 1.0);             \n \
                                            vec4 normal   = WorldMatrix * vec4(vert_normal, 0.0);               \n \
                                                                                                                \n \
                                            frag_color    = Color;                                              \n \
                                            frag_position = position.xyz;                                       \n \
                                            frag_normal   = normal.xyz;                                         \n \
                                        }											                            \n";


    const std::string gFragmentShader =	"#version 330									                                            \n \
                                                                                                                                    \n \
                                        layout(std140) uniform Light                                                                \n \
                                        {                                                                                           \n \
                                            vec4  AmbientLight;                                                                     \n \
                                            vec4  LightIntensity;                                                                   \n \
                                            vec3  LightPosition;                                                                    \n \
                                            float LightAttenuation;                                                                 \n \
                                        };                                                                                          \n \
                                                                                                                                    \n \
                                        in vec4 frag_color;                                                                         \n \
                                        in vec3 frag_position;                                                                      \n \
                                        in vec3 frag_normal;                                                                        \n \
                                                                                                                                    \n \
                                        out vec4 finalColor;						                                                \n \
                                                                                                                                    \n \
                                        void CalcLightIntensity(out vec3 positionDiff, out vec4 attenIntensity)                     \n \
                                        {                                                                                           \n \
                                            positionDiff   = LightPosition - frag_position;                                         \n \
                                            float distance = length(positionDiff);                                                  \n \
                                            attenIntensity = LightIntensity * (1 / (1.0 + (distance * LightAttenuation)));          \n \
                                        }                                                                                           \n \
                                                                                                                                    \n \
                                                                                                                                    \n \
                                        void main()										                                            \n \
                                        {												                                            \n \
                                            vec3 positionDiff   = vec3(0.0f);                                                       \n \
                                            vec4 attenIntensity = vec4(0.0f);                                                       \n \
                                            CalcLightIntensity(positionDiff, attenIntensity);                                       \n \
                                                                                                                                    \n \
                                            vec3 dirToLight    = normalize(positionDiff);                                           \n \
                                            float angIncidence = dot(normalize(frag_normal), dirToLight);                           \n \
                                            angIncidence       = clamp(angIncidence, 0, 1);                                         \n \
                                                                                                                                    \n \
                                            finalColor = (frag_color * attenIntensity * angIncidence) +                             \n \
                                                         (frag_color * AmbientLight);                                               \n \
                                        }												                                            \n";
}