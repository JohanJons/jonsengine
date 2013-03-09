#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								\n	\
                                                                                    \n  \
                                        layout(std140) uniform DefaultUniform       \n  \
                                        {                                           \n  \
                                            vec4 Color;                             \n  \
                                            mat4 MVPMatrix;                         \n  \
                                        };                                          \n  \
                                                                                    \n	\
                                        in vec3 in_position;						\n	\
                                        out vec3 ex_color;                          \n	\
                                        void main()									\n	\
                                        {											\n	\
                                            vec4 camPos = vec4(in_position, 1.0f);	\n	\
                                            ex_color = Color.xyz;                   \n  \
                                            gl_Position = MVPMatrix * camPos;       \n  \
                                        }											\n";


    const std::string gFragmentShader =	"#version 330									\n \
                                        in vec3 ex_color;								\n \
                                        out vec4 outColor;								\n \
                                        void main()										\n \
                                        {												\n \
                                            outColor = vec4(ex_color, 1.0f);	        \n \
                                        }												\n";


    struct ShaderData
    {
        Vec4 Color;
        Mat4 MVPMatrix;

        void CopyUniformData(std::vector<float>& buffer) const
        {
            std::vector<float> colorValues = GetTypeValues(Color);
            std::vector<float> mvpMatrixValues = GetTypeValues(MVPMatrix);

            buffer.insert(buffer.end(), colorValues.begin(), colorValues.end());
            buffer.insert(buffer.end(), mvpMatrixValues.begin(), mvpMatrixValues.end());
        }
    };


}