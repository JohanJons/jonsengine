#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								                            \n	\
                                                                                                                \n  \
                                        layout(std140) uniform DefaultUniform                                   \n  \
                                        {                                                                       \n  \
                                            vec4 Color;                                                         \n  \
                                            mat4 WVPMatrix;                                                     \n  \
                                            mat4 WorldMatrix;                                                   \n  \
                                                                                                                \n  \
                                            vec4 AmbientLight;                                                  \n  \
                                            vec4 LightIntensity;                                                \n  \
                                            vec3 DirToLight;                                                    \n  \
                                        };                                                                      \n  \
                                                                                                                \n	\
                                        layout(location = 0) in vec3 vert_position;				                \n	\
                                        layout(location = 1) in vec3 vert_normal;					            \n	\
                                                                                                                \n	\
                                        out vec4 frag_color;                                                    \n	\
                                        out vec4 frag_position;                                                 \n	\
                                        out vec4 frag_normal;                                                   \n	\
                                                                                                                \n	\
                                        out vec4 frag_ambientLight;                                             \n	\
                                        out vec4 frag_lightIntensity;                                           \n	\
                                        out vec4 frag_dirToLight;                                               \n	\
                                                                                                                \n	\
                                        void main()									                            \n	\
                                        {											                            \n	\
                                            gl_Position = WVPMatrix * vec4(vert_position, 1.0);                 \n  \
                                                                                                                \n  \
                                            frag_color    = Color;                                              \n  \
                                            frag_position = WorldMatrix * vec4(vert_position, 1.0);             \n  \
                                            frag_normal   = normalize(WorldMatrix * vec4(vert_normal, 1.0));    \n  \
                                                                                                                \n  \
                                            frag_ambientLight   = AmbientLight;                                 \n  \
                                            frag_lightIntensity = LightIntensity;                               \n  \
                                            frag_dirToLight     = normalize(vec4(DirToLight, 1.0));             \n  \
                                        }											                            \n";


    const std::string gFragmentShader =	"#version 330									                        \n \
                                                                                                                \n \
                                        in vec4 frag_color;                                                     \n \
                                        in vec4 frag_position;                                                  \n \
                                        in vec4 frag_normal;                                                    \n \
                                                                                                                \n \
                                        in vec4 frag_ambientLight;                                              \n \
                                        in vec4 frag_lightIntensity;                                            \n \
                                        in vec4 frag_dirToLight;                                                \n \
                                                                                                                \n \
                                        out vec4 finalColor;						                            \n \
                                                                                                                \n \
                                        void main()										                        \n \
                                        {												                        \n \
                                            float angIncidence = dot(frag_normal, frag_dirToLight);             \n  \
                                            angIncidence = clamp(angIncidence, 0, 1);                           \n  \
                                                                                                                \n  \
                                            finalColor = (frag_lightIntensity * frag_color * angIncidence) +    \n  \
                                                         (frag_color * frag_ambientLight);                      \n  \
                                        }												                        \n";
}