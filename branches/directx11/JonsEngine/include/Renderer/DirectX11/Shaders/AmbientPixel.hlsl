#ifndef AMBIENT_PIXEL_HLSL
#define AMBIENT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer AmbientConstants : register(b0)
{
    float4 gAmbientLight;
    float2 gScreenSize;
};

Texture2D gDiffuseTexture : register(t1);
SamplerState gSampler : register(s0);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float2 texcoord = position.xy / gScreenSize;

    return gDiffuseTexture.Sample(gSampler, texcoord) * gAmbientLight;
}

#endif