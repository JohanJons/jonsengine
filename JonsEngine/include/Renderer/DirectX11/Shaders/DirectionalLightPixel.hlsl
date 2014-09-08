#ifndef DIRECTIONAL_LIGHT_PIXEL_HLSL
#define DIRECTIONAL_LIGHT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"
#include "Constants.hlsl"

#define DEPTH_BIAS 0.005
#define NUM_CASCADES 4

cbuffer DirectionalLightConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gSplitVPMatrices[NUM_CASCADES];
    float4x4 gCameraViewMatrix;
    float4 gSplitDistances;
    float4 gLightColor;
    float4 gLightDirection;
};

Texture2D gPositionTexture : register(TEXTURE_REGISTER_POSITION);
Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
Texture2DArray gShadowmap : register(TEXTURE_REGISTER_DEPTH);
SamplerComparisonState gShadowmapSampler : register(SAMPLER_REGISTER_DEPTH);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    float4 worldPos = gPositionTexture[uint2(position.xy)];
    float4 diffuse = gDiffuseTexture[uint2(position.xy)];
    float4 normal = gNormalTexture[uint2(position.xy)];

    float4 camPos = mul(gCameraViewMatrix, worldPos);

    uint index = 3;
    if (camPos.z > gSplitDistances.x)
        index = 0;
    else if (camPos.z > gSplitDistances.y)
        index = 1;
    else if (camPos.z > gSplitDistances.z)
        index = 2;

    float3 projCoords = (float3)mul(gSplitVPMatrices[index], worldPos);
    const float2 texelSize = 1.0 / float2(1024.0, 1024.0);
    projCoords.xy = (floor(projCoords.xy / texelSize)) * texelSize;

    float viewDepth = projCoords.z - DEPTH_BIAS;
    projCoords.z = float(index);
    float visibilty = gShadowmap.SampleCmpLevelZero(gShadowmapSampler, projCoords, viewDepth);

    float angleNormal = clamp(dot(normal, gLightDirection), 0, 1);

    return visibilty * diffuse * angleNormal * gLightColor;
}

#endif