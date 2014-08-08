#ifndef POINT_LIGHT_PIXEL_HLSL
#define POINT_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer PointLightConstants : register(b0)
{
    float4x4 gLightWVPMatrix;
    float4 gLightColor;
    float4 gLightPosition;
    float gLightIntensity;
    float gMaxDistance;
};

Texture2D gPositionTexture : register(t0);
Texture2D gDiffuseTexture : register(t1);
Texture2D gNormalTexture : register(t2);
TextureCube gShadowmap : register(t3);


float VectorToDepthValue(float3 Vec)
{
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 100.0;      // TODO: TEMP - use constant buffer values!
    const float n = 0.1;
    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;

    return (NormZComp + 1.0) * 0.5;
}

float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float4 worldPosition = gPositionTexture[uint2(position.xy)];
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float3 normal = (float3)gNormalTexture[uint2(position.xy)];

    float3 positionDiff = (float3)(gLightPosition - worldPosition);
    float distance = length(positionDiff);
    float attenuation = clamp(1.0 - distance * distance * (1 / (gMaxDistance * gMaxDistance)), 0.0, 1.0);
    attenuation *= attenuation;

    float3 lightDir = normalize(positionDiff);
    float angleNormal = clamp(dot(normalize(normal), lightDir), 0, 1);

    return diffuse * angleNormal * attenuation * gLightIntensity * gLightColor;
}

#endif