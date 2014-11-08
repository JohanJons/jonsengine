#ifndef POINT_LIGHT_PIXEL_HLSL
#define POINT_LIGHT_PIXEL_HLSL

#include "Constants.h"
#include "Common.hlsl"

cbuffer PointLightConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gInvProjMatrix;
    float4 gLightColor;
    float4 gViewLightPosition;
    float2 gScreenSize;
    float gLightIntensity;
    float gMaxDistance;
    float gZFar;
    float gZNear;
};

Texture2D gDepthTexture : register(TEXTURE_REGISTER_DEPTH);
Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
TextureCube gShadowmap : register(TEXTURE_REGISTER_EXTRA);
SamplerState gShadowmapSampler : register(SAMPLER_REGISTER_POINT);


float VectorToDepthValue(const float3 Vec)
{
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = gZFar;
    const float n = gZNear;

    float NormZComp = -(f / (n - f) - (n * f) / (n - f) / LocalZcomp);

    return NormZComp;
}

float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float depth = gDepthTexture[uint2(position.xy)].r;
    float4 viewPosition = float4(reconstructViewPosition(depth, float2(position.x / gScreenSize.x, position.y / gScreenSize.y), gInvProjMatrix), 1.0);
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float3 normal = (float3)gNormalTexture[uint2(position.xy)];
    float3 positionDiff = (float3)(gViewLightPosition - viewPosition);

    // shadowmapping
    float3 cubemapDir = (float3)(viewPosition - gViewLightPosition);
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