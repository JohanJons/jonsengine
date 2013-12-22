#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gStencilVertexShader =
    "#version 330                                                                           \n \
                                                                                            \n \
    layout(std140) uniform;                                                                 \n \
                                                                                            \n \
    uniform UnifStencil                                                                     \n \
    {                                                                                       \n \
        mat4 mWVPMatrix;                                                                    \n \
    } UnifStencilPass;                                                                      \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = UnifStencilPass.mWVPMatrix * vec4(vert_position, 1.0);                \n \
    }                                                                                       \n";
}