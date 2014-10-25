#ifndef SSAO_PIXEL_HLSL
#define SSAO_PIXEL_HLSL

#include "Constants.h"
#include "Common.hlsl"

static const float gNumSamples = 11.0;
static const float gRadius = 0.2;
static const float gRadius2 = gRadius * gRadius;
static const float gProjScale = 500.0;
static const float gNumSpiralTurns = 7;
static const float gBias = 0.02;
static const float gIntensity = 1.0;
static const float FAR_PLANE_Z = 100.0f; // TODO - variable


cbuffer SSAOCBuffer : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gViewMatrix;
};

Texture2D gPositionTexture : register(TEXTURE_REGISTER_POSITION);
SamplerState gPointSampler : register(SAMPLER_REGISTER_POINT);


float3 reconstructNormal(float3 positionWorldSpace)
{
    return normalize(cross(ddy(positionWorldSpace), ddx(positionWorldSpace)));
}

/** Read the camera - space position of the point at screen - space pixel ssP + unitOffset * ssR.Assumes length(unitOffset) == 1 */
float3 getOffsetPosition(int2 ssC, float2 unitOffset, float ssR) {
    // Derivation:
    //  mipLevel = floor(log(ssR / MAX_OFFSET));

    // TODO: mip levels
    int mipLevel = 0; //TODO: clamp((int)floor(log2(ssR)) - LOG_MAX_OFFSET, 0, MAX_MIP_LEVEL);

    int2 ssP = int2(ssR*unitOffset) + ssC;

    float3 P = gPositionTexture[ssP].xyz;

    // Divide coordinate by 2^mipLevel
    //P = gPositionTexture.Load(int3(ssP >> mipLevel, mipLevel)).xyz;
    P = mul(gViewMatrix, float4(P, 1.0)).xyz;

    return P;
}

float2 tapLocation(int sampleNumber, float spinAngle, out float ssR)
{
    // Radius relative to ssR
    float alpha = float(sampleNumber + 0.5) * (1.0 / gNumSamples);
    float angle = alpha * (gNumSpiralTurns * 6.28) + spinAngle;

    ssR = alpha;
    return float2(cos(angle), sin(angle));
}

float sampleAO(uint2 screenSpacePos, float3 originPos, float3 normal, float ssDiskRadius, int tapIndex, float randomPatternRotationAngle)
{
    float ssR;
    float2 unitOffset = tapLocation(tapIndex, randomPatternRotationAngle, ssR);
    ssR *= ssDiskRadius;

    // The occluding point in camera space
    float3 Q = getOffsetPosition(screenSpacePos, unitOffset, ssR);

    float3 v = Q - originPos;

    float vv = dot(v, v);
    float vn = dot(v, normal);

    const float epsilon = 0.01;
    float f = max(gRadius2 - vv, 0.0); 
    
    return f * f * f * max((vn - gBias) / (epsilon + vv), 0.0);
}

float ps_main(float4 position : SV_Position) : SV_Target0
{
    uint2 screenSpacePos = (uint2)position.xy;

    float3 originPos = gPositionTexture[screenSpacePos].xyz;
    originPos = mul(gViewMatrix, float4(originPos, 1.0)).xyz;
    float3 normal = reconstructNormal(originPos);

    // Hash function used in the HPG12 AlchemyAO paper
    float randomPatternRotationAngle = (3 * screenSpacePos.x ^ screenSpacePos.y + screenSpacePos.x * screenSpacePos.y) * 10;
    float ssDiskRadius = -gProjScale * gRadius / originPos.z;

    float ao = 0.0;
    for (int i = 0; i < gNumSamples; i++)
    {
        ao += sampleAO(screenSpacePos, originPos, normal, ssDiskRadius, i, randomPatternRotationAngle);
    }

    float temp = gRadius2 * gRadius;
    ao /= temp * temp;

    float A = max(0.0, 1.0 - ao * gIntensity * (5.0 / gNumSamples));

    // Bilateral box-filter over a quad for free, respecting depth edges
    // (the difference that this makes is subtle)
    if (abs(ddx(originPos.z)) < 0.02) {
        A -= ddx(A) * ((screenSpacePos.x & 1) - 0.5);
    }
    if (abs(ddy(originPos.z)) < 0.02) {
        A -= ddy(A) * ((screenSpacePos.y & 1) - 0.5);
    }

    return A;
}

#endif