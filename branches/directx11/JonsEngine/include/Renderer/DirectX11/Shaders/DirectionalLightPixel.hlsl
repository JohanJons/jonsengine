#ifndef DIRECTIONAL_LIGHT_PIXEL_HLSL
#define DIRECTIONAL_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer DirectionalLightConstants : register(b0)
{
    float4 gLightColor;
    float4 gLightDirection;
};

Texture2D gDiffuseTexture : register(t1);
Texture2D gNormalTexture : register(t2);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float4 normal = gNormalTexture[uint2(position.xy)];

    float angleNormal = clamp(dot(normal, gLightDirection), 0, 1);

    return diffuse * angleNormal * gLightColor;
}

#endif