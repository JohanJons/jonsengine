#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gDirLightVertexShader =
    "#version 430                                                                           \n \
                                                                                            \n \
    layout(std140) uniform;                                                                 \n \
                                                                                            \n \
    uniform UnifDirLight                                                                    \n \
    {                                                                                       \n \
        mat4 mVPMatrix;                                                                     \n \
        vec4 mLightColor;                                                                   \n \
        vec4 mLightDir;                                                                     \n \
        vec4 mGamma;                                                                        \n \
        vec2 mScreenSize;                                                                   \n \
    } UnifDirLightPass;                                                                     \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = vec4(vert_position, 1.0);                                             \n \
    }                                                                                       \n";
}