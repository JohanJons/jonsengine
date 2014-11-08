#ifndef DIRECTIONAL_LIGHT_PIXEL_HLSL
#define DIRECTIONAL_LIGHT_PIXEL_HLSL

#include "Constants.h"
#include "FullscreenTriangleVertex.hlsl"
#include "Common.hlsl"

#define DEPTH_BIAS 0.00005
#define NUM_CASCADES 4

cbuffer DirectionalLightConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gSplitVPMatrices[NUM_CASCADES];
    float4x4 gInvProjMatrix;
    float4 gSplitDistances;
    float4 gLightColor;
    float4 gLightDirection;
    float2 gScreenSize;
    float gShadowmapSize;
};

Texture2D gDepthTexture : register(TEXTURE_REGISTER_DEPTH);
Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
Texture2DArray gShadowmap : register(TEXTURE_REGISTER_EXTRA);
SamplerComparisonState gShadowmapSampler : register(SAMPLER_REGISTER_POINT_COMPARE);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float depth = gDepthTexture[uint2(position.xy)].r;
    float4 viewPosition = float4(reconstructViewPosition(depth, float2(position.x / gScreenSize.x, position.y / gScreenSize.y), gInvProjMatrix), 1.0);
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float4 normal = gNormalTexture[uint2(position.xy)];

    uint index = 3;
    if (viewPosition.z > gSplitDistances.x)
        index = 0;
    else if (viewPosition.z > gSplitDistances.y)
        index = 1;
    else if (viewPosition.z > gSplitDistances.z)
        index = 2;

    float3 projCoords = (float3)mul(gSplitVPMatrices[index], viewPosition);
    const float2 texelSize = 1.0 / float2(gShadowmapSize, gShadowmapSize);
    projCoords.xy = (floor(projCoords.xy / texelSize)) * texelSize;

    float viewDepth = projCoords.z - DEPTH_BIAS;
    projCoords.z = float(index);

    float inc = 1.0 / gShadowmapSize;
    float visibilty = 0.0;
    visibilty += gShadowmap.SampleCmpLevelZero(gShadowmapSampler, projCoords + float3(inc, 0, 0), viewDepth);
    visibilty += gShadowmap.SampleCmpLevelZero(gShadowmapSampler, projCoords - float3(inc, 0, 0), viewDepth);
    visibilty += gShadowmap.SampleCmpLevelZero(gShadowmapSampler, projCoords + float3(0, inc, 0), viewDepth);
    visibilty += gShadowmap.SampleCmpLevelZero(gShadowmapSampler, projCoords - float3(0, inc, 0), viewDepth);
    visibilty /= 4;

    float angleNormal = clamp(dot(normal, gLightDirection), 0, 1);

    return visibilty * diffuse * angleNormal * gLightColor;
}

#endif