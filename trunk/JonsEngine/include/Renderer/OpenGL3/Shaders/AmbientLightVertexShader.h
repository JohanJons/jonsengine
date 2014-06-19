#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gAmbientVertexShader =
    "#version 420                                                                           \n \
                                                                                            \n \
    layout(std140) uniform UnifAmbient                                                      \n \
    {                                                                                       \n \
        vec4 mLightColor;                                                                   \n \
        vec4 mGamma;                                                                        \n \
        vec2 mScreenSize;                                                                   \n \
        int mNumSamples;                                                                    \n \
    } UnifAmbientPass;                                                                      \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = vec4(vert_position, 1.0);                                             \n \
    }                                                                                       \n";
}