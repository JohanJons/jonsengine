#ifndef AMBIENT_PIXEL_HLSL
#define AMBIENT_PIXEL_HLSL

#include "Constants.h"

cbuffer AmbientConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4 gAmbientLight;
    bool gUseSSAO;
};

Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gSSAOTexture : register(TEXTURE_REGISTER_EXTRA);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float ssao = 1.0;
    if (gUseSSAO)
    {
        ssao = gSSAOTexture[uint2(position.xy)].r;
        ssao = (clamp(1.0 - (1.0 - ssao), 0.0, 1.0) + 0.1) / (1.0 + 0.1);
    }

    return gDiffuseTexture[uint2(position.xy)] * ssao * gAmbientLight;
}

#endif