#ifndef DIRECTIONAL_LIGHT_PIXEL_HLSL
#define DIRECTIONAL_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer DirectionalLightConstants : register(b0)
{
    float4 gLightColor;
    float4 gLightDirection;
    float2 gScreenSize;
};

Texture2D gDiffuseTexture : register(t1);
Texture2D gNormalTexture : register(t2);
SamplerState gSampler : register(s0);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float2 texcoord = position.xy / gScreenSize;
    float4 diffuse = gDiffuseTexture.Sample(gSampler, texcoord);
    float4 normal = gNormalTexture.Sample(gSampler, texcoord);

    float angleNormal = clamp(dot(normal, gLightDirection), 0, 1);

    return diffuse * angleNormal * gLightColor;
}

#endif