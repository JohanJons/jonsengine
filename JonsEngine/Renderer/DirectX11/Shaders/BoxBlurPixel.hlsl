#ifndef BOX_BLUR_PIXEL_HLSL
#define BOX_BLUR_PIXEL_HLSL

#include "Constants.h"

cbuffer BoxBlurConstants : register(CBUFFER_REGISTER_PIXEL)
{
    // (1 / TEXTURE_WIDTH, 0) for horizontal pass, (0, 1 / TEXTURE_HEIGHT) for vertical pass
    float2 gTexelSize;
};

Texture2D gRawAOTexture : register(TEXTURE_REGISTER_EXTRA);

static const uint NUM_SAMPLES = 7;


float ps_main(float4 position : SV_Position) : SV_Target0
{
    float ret = 0.0;

    for (int i = -3; i <= 3; i++)
    {
        const uint2 texCoord = position.xy + i;
        ret += gRawAOTexture[texCoord].r;
    }

    ret /= NUM_SAMPLES;

    return ret;
}

#endif