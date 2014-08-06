#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

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
    float4 mWorldPosition : WORLD_POSITION;
    float4 mNormal : NORMAL;
    float4 mTangent : TANGENT;
    float4 mBitangent : BITANGENT;
    float2 mTexcoord : TEXCOORD;
};

cbuffer GBufferConstants : register(b0)
{
    float4x4 gWVPMatrix;
    float4x4 gWorldMatrix;
    float gTextureTilingFactor;
    bool gHasDiffuseTexture;
    bool gHasNormalTexture;
};

Texture2D gDiffuseTexture : register(t1);
Texture2D gNormalTexture : register(t2);
SamplerState gSampler : register(s0);


GBufferVSOut vs_main(GBufferVSIn input)
{
    GBufferVSOut output;

    output.mPosition = mul(gWVPMatrix, float4(input.mPosition, 1.0f));
    output.mWorldPosition = mul(gWorldMatrix, float4(input.mPosition, 1.0f));
    output.mNormal = mul(gWorldMatrix, float4(input.mNormal, 0.0f));
    output.mTangent = mul(gWorldMatrix, float4(input.mTangent, 0.0f));
    output.mBitangent = mul(gWorldMatrix, float4(input.mBitangent, 0.0f));
    output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}

#endif