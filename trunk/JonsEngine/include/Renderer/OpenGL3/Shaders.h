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
                                            mat4 MVPMatrix;                                                     \n  \
                                            mat4 MVMatrix;                                                      \n  \
                                                                                                                \n  \
                                            vec4 AmbientLight;                                                  \n  \
                                            vec4 LightIntensity;                                                \n  \
                                            vec3 DirToLight;                                                    \n  \
                                        };                                                                      \n  \
                                                                                                                \n	\
                                        layout(location = 0) in vec3 vert_position;				                \n	\
                                        layout(location = 1) in vec3 vert_normal;					            \n	\
                                        out vec4 frag_color;                                                    \n	\
                                                                                                                \n	\
                                        void main()									                            \n	\
                                        {											                            \n	\
                                            gl_Position = MVPMatrix * vec4(vert_position, 1.0);                 \n  \
                                                                                                                \n  \
                                             vec4 normCamSpace = normalize(MVMatrix * vec4(vert_normal, 1.0));  \n  \
                                             vec4 dirToLight = normalize(vec4(DirToLight, 1.0));                \n  \
                                            float angIncidence = dot(normCamSpace, dirToLight);                 \n  \
                                            angIncidence = clamp(angIncidence, 0, 1);                           \n  \
                                                                                                                \n  \
                                            frag_color = (LightIntensity * Color * angIncidence) +              \n  \
                                                         (Color * AmbientLight);                                \n  \
                                        }											                            \n";


    const std::string gFragmentShader =	"#version 330									\n \
                                                                                        \n \
                                        in vec4 frag_color;								\n \
                                        out vec4 finalColor;						    \n \
                                                                                        \n \
                                        void main()										\n \
                                        {												\n \
                                            finalColor = clamp(frag_color, 0, 1);	    \n \
                                        }												\n";
}