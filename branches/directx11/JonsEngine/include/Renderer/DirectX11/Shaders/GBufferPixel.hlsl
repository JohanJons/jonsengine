#ifndef GBUFFER_PIXEL_HLSL
#define GBUFFER_PIXEL_HLSL

#define GBUFFER_CONSTANTS
#include "Constants.hlsl"

Texture2D gDiffuseTexture : register(t0);
SamplerState gSampler : register(s0);

GBufferPSOut main(GBufferVSOut input)
{
    GBufferPSOut output;

    output.mWorldPosition = input.mWorldPosition;
    output.mDiffuse = (float3)gDiffuseTexture.Sample(gSampler, input.mTexcoord);

    return output;
}

#endif