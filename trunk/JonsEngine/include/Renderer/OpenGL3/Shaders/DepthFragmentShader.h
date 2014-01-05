#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gDepthFragmentShader =
    "#version 430                                                                                                                                               \n \
                                                                                                                                                                \n \
    layout(std140) uniform;                                                                                                                                     \n \
                                                                                                                                                                \n \
    uniform UnifDepth                                                                                                                                           \n \
    {                                                                                                                                                           \n \
        mat4 mWVPMatrix;                                                                                                                                        \n \
        vec2 mScreenSize;                                                                                                                                       \n \
        float mZNear;                                                                                                                                           \n \
        float mZFar;                                                                                                                                            \n \
    } UnifDepthPass;                                                                                                                                            \n \
                                                                                                                                                                \n \
    layout (binding = 5) uniform sampler2D unifDepthTexture;                                                                                                    \n \
                                                                                                                                                                \n \
    out vec4 fragColor;                                                                                                                                         \n \
                                                                                                                                                                \n \
    void main()                                                                                                                                                 \n \
    {                                                                                                                                                           \n \
        vec2 texcoord = gl_FragCoord.xy / UnifDepthPass.mScreenSize;                                                                                            \n \
                                                                                                                                                                \n \
        float depthValue = texture(unifDepthTexture, texcoord).x;                                                                                               \n \
        depthValue = (2.0 * UnifDepthPass.mZNear) / (UnifDepthPass.mZFar + UnifDepthPass.mZNear - depthValue * (UnifDepthPass.mZFar - UnifDepthPass.mZNear));   \n \
                                                                                                                                                                \n \
        fragColor = vec4(depthValue, depthValue, depthValue, 1.0);                                                                                              \n \
    }                                                                                                                                                           \n";
}