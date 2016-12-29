#ifndef SIMPLE_TEXTURE_PIXEL_HLSL
#define SIMPLE_TEXTURE_PIXEL_HLSL

#include "Constants.h"

Texture2D gTexture : register(TEXTURE_REGISTER_DIFFUSE);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    const uint2 screenSpacePos = (uint2)position.xy;

    return gTexture[screenSpacePos];
}

#endif