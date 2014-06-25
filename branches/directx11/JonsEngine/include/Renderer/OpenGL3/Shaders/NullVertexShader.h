#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gNullVertexShader =
    "#version 420                                                                           \n \
                                                                                            \n \
    layout(std140) uniform UnifNull                                                         \n \
    {                                                                                       \n \
        mat4 mWVPMatrix;                                                                    \n \
    } UnifNullPass;                                                                         \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = UnifNullPass.mWVPMatrix * vec4(vert_position, 1.0);                   \n \
    }                                                                                       \n";
}