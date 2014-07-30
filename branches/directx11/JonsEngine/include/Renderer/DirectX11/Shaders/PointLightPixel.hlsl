#ifndef POINT_LIGHT_PIXEL_HLSL
#define POINT_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer PointLightConstants : register(b0)
{
    float4 gAmbientLight;
    float2 gScreenSize;
};

Texture2D gDiffuseTexture : register(t0);
SamplerState gSampler : register(s0);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float2 texcoord = position.xy / gScreenSize;

    return float4(0.0, 0.0, 0.0, 0.0);
}

#endif