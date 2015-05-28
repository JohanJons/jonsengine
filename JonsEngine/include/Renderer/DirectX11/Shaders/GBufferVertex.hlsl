#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

#include "Constants.h"

struct GBufferVSIn
{
    float3 mPosition : POSITION;
    float3 mNormal : NORMAL;
    float3 mTangent : TANGENT;
    float3 mBitangent : BITANGENT;
    float2 mTexcoord : TEXCOORD;
};

struct GBufferVSOut
{
    float4 mPosition : SV_POSITION;
    float3 mNormal : NORMAL;
    float3 mTangent : TANGENT;
    float3 mBitangent : BITANGENT;
    float2 mTexcoord : TEXCOORD;
};

cbuffer GBufferConstants : register(CBUFFER_REGISTER_VERTEX)
{
    float4x4 gWVPMatrix;
    // Something bogus with glm makes any non-4x4 matrix bad when uploaded to shader. Probably simd/non-simd related
    // Otherwise, 3x3 would be enough.
    float4x4 gWorldViewMatrix;
    float gTextureTilingFactor;
    bool gHasDiffuseTexture;
    bool gHasNormalTexture;
};


GBufferVSOut vs_main(GBufferVSIn input)
{
    GBufferVSOut output;

    output.mPosition = mul(gWVPMatrix, float4(input.mPosition, 1.0));
    output.mNormal = mul((float3x3)gWorldViewMatrix, input.mNormal);
    if (gHasNormalTexture)
    {
        output.mTangent = mul((float3x3)gWorldViewMatrix, input.mTangent);
        output.mBitangent = mul((float3x3)gWorldViewMatrix, input.mBitangent);
    }
    if (gHasDiffuseTexture)
        output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}

#endif