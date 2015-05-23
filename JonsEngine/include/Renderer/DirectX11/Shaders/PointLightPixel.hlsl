#ifndef POINT_LIGHT_PIXEL_HLSL
#define POINT_LIGHT_PIXEL_HLSL

#include "Constants.h"
#include "Common.hlsl"
#include "Shadowmapping.hlsl"

#define DEPTH_BIAS 0.0005

cbuffer PointLightConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gInvProjMatrix;
    float4 gLightColor;
    float4 gViewLightPosition;
    float2 gWindowSize;
    float gLightIntensity;
    float gZNear;
    float gLightRadius;
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

    const float f = gLightRadius;
    const float n = gZNear;

    float NormZComp = -(f / (n - f) - (n * f) / (n - f) / LocalZcomp);

    return NormZComp;
}

float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float depth = gDepthTexture[uint2(position.xy)].r;
    float4 viewPosition = float4(ReconstructViewPosition(depth, float2(position.x / gWindowSize.x, position.y / gWindowSize.y), gInvProjMatrix), 1.0);
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float3 normal = SampleNormalTexture(gNormalTexture, uint2(position.xy));

    float3 positionDiff = (float3)(gViewLightPosition - viewPosition);
    float3 lightDir = normalize(positionDiff);
    const float normalLightAngle = dot(normal, lightDir);

    // shadowmapping
    float3 cubemapDir = (float3)(viewPosition - gViewLightPosition);
    cubemapDir.z = -cubemapDir.z;       // TODO: any way to remove this extra instruction?
    float storedDepth = gShadowmap.Sample(gShadowmapSampler, cubemapDir).r;
    float visibility = 0.0;
    if (storedDepth - DEPTH_BIAS > VectorToDepthValue(cubemapDir))
        visibility = 1.0;

    // light attenuation
    float distance = length(positionDiff);
    float attenuation = clamp(1.0 - distance * distance * (1 / (gLightRadius * gLightRadius)), 0.0, 1.0);
    attenuation *= attenuation;

    float angleNormal = clamp(normalLightAngle, 0, 1);

    return visibility * diffuse * angleNormal * attenuation * gLightIntensity * gLightColor;
}

#endif