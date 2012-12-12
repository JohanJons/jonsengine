#pragma once

#include "include/Core/Utils/Types.h"

#include <string>

using namespace JonsEngine;

namespace JonsGame
{
    const std::string gVertexShader =	"#version 330								\n	\
                                                                                    \n  \
                                        layout(std140) uniform Uni                  \n  \
                                        {                                           \n  \
                                            vec4 mColor;                            \n  \
                                            vec2 mOffset;                           \n  \
                                            mat4 mPerspMatrix;                      \n  \
                                        };                                          \n  \
                                                                                    \n	\
                                        in vec3 in_position;						\n	\
                                        out vec3 ex_color;                          \n	\
                                        void main()									\n	\
                                        {											\n	\
                                            vec4 camPos = vec4(in_position, 1.0f) + vec4(mOffset.x, mOffset.y, 0.0, 0.0);	\n	\
                                                                                    \n \
                                            gl_Position = mPerspMatrix * camPos;    \n \
                                            ex_color = mColor.xyz;                  \n \
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
        Vec2 mOffset;
        Mat4 mPerspMatrix;

        void CopyUniformData(uint8_t* destination) const
        {
            size_t index = 0;
            memcpy(destination, GetTypeValue(mColor), sizeof(mColor));
            index += sizeof(mColor);
            memcpy(destination+index, GetTypeValue(mOffset), sizeof(mOffset));
            index += sizeof(mOffset);
            memcpy(destination+index, GetTypeValue(mPerspMatrix), sizeof(mPerspMatrix));
        }
    };
}