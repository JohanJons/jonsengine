#ifndef POINT_LIGHT_PIXEL_HLSL
#define POINT_LIGHT_PIXEL_HLSL

#include "Constants.h"

cbuffer PointLightConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4 gLightColor;
    float4 gLightPosition;
    float gLightIntensity;
    float gMaxDistance;
};

Texture2D gPositionTexture : register(TEXTURE_REGISTER_POSITION);
Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
TextureCube gShadowmap : register(TEXTURE_REGISTER_DEPTH);
SamplerState gShadowmapSampler : register(SAMPLER_REGISTER_DEPTH_NO_COMPARE);


float VectorToDepthValue(float3 Vec)
{
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 100.0;
    const float n = 0.1;

    float NormZComp = -(f / (n - f) - (n * f) / (n - f) / LocalZcomp);

    return NormZComp;
}

float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float4 worldPosition = gPositionTexture[uint2(position.xy)];
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float3 normal = (float3)gNormalTexture[uint2(position.xy)];
    float3 positionDiff = (float3)(gLightPosition - worldPosition);

    // shadowmapping
    float3 cubemapDir = (float3)(worldPosition - gLightPosition);
    cubemapDir.z = -cubemapDir.z;       // TODO: any way to remove this extra instruction?
    float storedDepth = gShadowmap.Sample(gShadowmapSampler, cubemapDir).r;
    float visibility = 0.0;
    if (storedDepth + 0.0001 > VectorToDepthValue(cubemapDir))
        visibility = 1.0;

    // light attenuation
    float distance = length(positionDiff);
    float attenuation = clamp(1.0 - distance * distance * (1 / (gMaxDistance * gMaxDistance)), 0.0, 1.0);
    attenuation *= attenuation;

    float3 lightDir = normalize(positionDiff);
    float angleNormal = clamp(dot(normalize(normal), lightDir), 0, 1);

    return visibility * diffuse * angleNormal * attenuation * gLightIntensity * gLightColor;
}

#endif