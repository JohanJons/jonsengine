#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gDepthVertexShader =
    "#version 430                                                                           \n \
                                                                                            \n \
    layout(std140) uniform;                                                                 \n \
                                                                                            \n \
    uniform UnifDepth                                                                       \n \
    {                                                                                       \n \
        mat4 mWVPMatrix;                                                                    \n \
        vec2 mScreenSize;                                                                   \n \
        float mZNear;                                                                       \n \
        float mZFar;                                                                        \n \
    } UnifDepthPass;                                                                        \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = UnifDepthPass.mWVPMatrix * vec4(vert_position, 1.0);                  \n \
    }                                                                                       \n";
}