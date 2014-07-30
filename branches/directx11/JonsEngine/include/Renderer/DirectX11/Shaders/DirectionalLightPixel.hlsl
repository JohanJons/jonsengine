#ifndef DIRECTIONAL_LIGHT_PIXEL_HLSL
#define DIRECTIONAL_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer DirectionalLightConstants : register(b0)
{
    float4x4 gLightViewProjectionMatrix;
    float4 gLightColor;
    float3 gLightDirection;
    float2 gScreenSize;
};

Texture2D gDiffuseTexture : register(t1);
Texture2D gNormalTexture : register(t2);
SamplerState gSampler : register(s0);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float2 texcoord = position.xy / gScreenSize;

    return float4(0.0, 0.0, 0.0, 0.0);
}

#endif