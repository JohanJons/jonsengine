#pragma once

#include "include/Core/Utils/Types.h"
#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
    const std::string gVertexShader =	"#version 330								\n	\
                                                                                    \n  \
                                        layout(std140) uniform DefaultUniform       \n  \
                                        {                                           \n  \
                                            vec4 mColor;                            \n  \
                                            mat4 mPerspMatrix;                      \n  \
                                        };                                          \n  \
                                                                                    \n	\
                                        in vec3 in_position;						\n	\
                                        out vec3 ex_color;                          \n	\
                                        void main()									\n	\
                                        {											\n	\
                                            vec4 camPos = vec4(in_position, 1.0f);	\n	\
                                            ex_color = mColor.xyz;                  \n  \
                                            gl_Position = mPerspMatrix * camPos;    \n  \
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
        Vec4 mColor;
        Mat4 mPerspMatrix;

        void CopyUniformData(vector<float>& buffer) const
        {
            vector<float> colorValues = GetTypeValues(mColor);
            vector<float> perspMatrixValues = GetTypeValues(mPerspMatrix);

            buffer.insert(buffer.end(), colorValues.begin(), colorValues.end());
            buffer.insert(buffer.end(), perspMatrixValues.begin(), perspMatrixValues.end());
        }
    };


}