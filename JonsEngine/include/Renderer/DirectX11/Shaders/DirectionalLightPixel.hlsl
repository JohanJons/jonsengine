#ifndef DIRECTIONAL_LIGHT_PIXEL_HLSL
#define DIRECTIONAL_LIGHT_PIXEL_HLSL

#include "Constants.h"
#include "Common.hlsl"
#include "Shadowmapping.hlsl"
#include "FullscreenTriangleVertex.hlsl"

#define NUM_CASCADES 4

cbuffer DirectionalLightConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gSplitVPMatrices[NUM_CASCADES];
    float4x4 gInvProjMatrix;
    float4 gSplitDistances;
    float4 gLightColor;
    float4 gLightDirection;
    float2 gWindowSize;
    float gShadowmapSize;
};

Texture2D gDepthTexture : register(TEXTURE_REGISTER_DEPTH);
Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
Texture2DArray gShadowmap : register(TEXTURE_REGISTER_EXTRA);
SamplerComparisonState gShadowmapSampler : register(SAMPLER_REGISTER_POINT_COMPARE);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    const float depth = gDepthTexture[uint2(position.xy)].r;
    float4 viewPosition = float4(ReconstructViewPosition(depth, float2(position.x / gWindowSize.x, position.y / gWindowSize.y), gInvProjMatrix), 1.0);
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float4 normal = gNormalTexture[uint2(position.xy)];

    uint index = 3;
    if (viewPosition.z > gSplitDistances.x)
        index = 0;
    else if (viewPosition.z > gSplitDistances.y)
        index = 1;
    else if (viewPosition.z > gSplitDistances.z)
        index = 2;

    const float normalLightAngle = dot(normal, gLightDirection);

    viewPosition.xyz += ShadowAcneNormalOffset(normal.xyz, normalLightAngle, gShadowmapSize);

    float3 projCoords = (float3)mul(gSplitVPMatrices[index], viewPosition);

    const float2 texelSize = 1.0 / float2(gShadowmapSize, gShadowmapSize);
    projCoords.xy = (floor(projCoords.xy / texelSize)) * texelSize;

    const float viewDepth = projCoords.z;
    projCoords.z = float(index);

    const float visibilty = gShadowmap.SampleCmpLevelZero(gShadowmapSampler, projCoords, viewDepth);
    const float angleNormal = clamp(normalLightAngle, 0, 1);

    return visibilty * diffuse * angleNormal * gLightColor;
}

#endif