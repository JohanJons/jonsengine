#pragma once

#include <string>

namespace JonsEngine
{
    const std::string gAmbientFragmentShader =
    "#version 420                                                                                                                   \n \
                                                                                                                                    \n \
    layout(std140) uniform UnifAmbient                                                                                              \n \
    {                                                                                                                               \n \
        vec4 mLightColor;                                                                                                           \n \
        vec4 mGamma;                                                                                                                \n \
        vec2 mScreenSize;                                                                                                           \n \
        int mNumSamples;                                                                                                            \n \
    } UnifAmbientPass;                                                                                                              \n \
                                                                                                                                    \n \
    layout (binding = 4) uniform sampler2DMS unifDiffuseTexture;                                                                    \n \
                                                                                                                                    \n \
    out vec4 fragColor;                                                                                                             \n \
                                                                                                                                    \n \
    void main()                                                                                                                     \n \
    {                                                                                                                               \n \
        ivec2 texcoord = ivec2(textureSize(unifDiffuseTexture) * (gl_FragCoord.xy / UnifAmbientPass.mScreenSize));                  \n \
                                                                                                                                    \n \
        vec3 diffuse = vec3(0.0);                                                                                                   \n \
        for (int i = 0; i < UnifAmbientPass.mNumSamples; i++)                                                                       \n \
            diffuse += texelFetch(unifDiffuseTexture, texcoord, i).rgb;                                                             \n \
        diffuse /= UnifAmbientPass.mNumSamples;                                                                                     \n \
                                                                                                                                    \n \
        fragColor = vec4(diffuse, 1.0) * UnifAmbientPass.mLightColor;                                                               \n \
    }                                                                                                                               \n";
}