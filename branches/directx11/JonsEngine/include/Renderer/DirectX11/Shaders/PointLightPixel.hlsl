#ifndef POINT_LIGHT_PIXEL_HLSL
#define POINT_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer PointLightConstants : register(b0)
{
    float4x4 gLightWVPMatrix;
    float4 gLightColor;
    float4 gLightPosition;
    float2 gScreenSize;
    float gLightIntensity;
    float gMaxDistance;
};

Texture2D gPositionTexture : register(t0);
Texture2D gDiffuseTexture : register(t1);
Texture2D gNormalTexture : register(t2);
TextureCube gShadowmap : register(t3);
SamplerState gSampler : register(s0);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float2 texcoord = position.xy / gScreenSize;
    float4 worldPosition = gPositionTexture.Sample(gSampler, texcoord);
    float4 diffuse = gDiffuseTexture.Sample(gSampler, texcoord);
    float3 normal = (float3)gNormalTexture.Sample(gSampler, texcoord);

    float3 positionDiff = (float3)(gLightPosition - worldPosition);
    float distance = length(positionDiff);
    float attenuation = clamp(1.0 - distance * distance * (1 / (gMaxDistance * gMaxDistance)), 0.0, 1.0);
    attenuation *= attenuation;

    float3 lightDir = normalize(positionDiff);
    float angleNormal = clamp(dot(normalize(normal), lightDir), 0, 1);

    return diffuse * angleNormal * attenuation * gLightIntensity * gLightColor;
}

#endif