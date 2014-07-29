#ifndef AMBIENT_PIXEL_HLSL
#define AMBIENT_PIXEL_HLSL

#define AMBIENT_CONSTANTS
#include "Constants.hlsl"

Texture2D gDiffuseTexture : register(t0);
SamplerState gSampler : register(s0);

float4 main(float4 position : SV_Position) : SV_Target0
{
    float2 texcoord = position.xy / gScreenSize;

    return gDiffuseTexture.Sample(gSampler, texcoord) * gAmbientLight;
}

#endif