#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gShadingVertexShader =
    "#version 430                                                                           \n \
                                                                                            \n \
    layout(std140) uniform;                                                                 \n \
                                                                                            \n \
    uniform UnifShading                                                                     \n \
    {                                                                                       \n \
        mat4 mWVPMatrix;                                                                    \n \
        vec4 mLightColor;                                                                   \n \
        vec4 mLightPosOrDir;                                                                \n \
        vec4 mGamma;                                                                        \n \
        vec2 mScreenSize;                                                                   \n \
                                                                                            \n \
        int   mLightType;                                                                   \n \
        float mFalloffFactor;                                                               \n \
        float mMaxDistance;                                                                 \n \
    } UnifShadingPass;                                                                      \n \
                                                                                            \n \
    layout(location = 0) in vec3 vert_position;                                             \n \
                                                                                            \n \
    void main()                                                                             \n \
    {                                                                                       \n \
        gl_Position = UnifShadingPass.mWVPMatrix * vec4(vert_position, 1.0);                \n \
    }                                                                                       \n";
}