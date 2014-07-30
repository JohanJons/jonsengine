#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

struct GBufferVSIn
{
    float3 mPosition : POSITION;
    float2 mTexcoord : TEXCOORD;
};

struct GBufferVSOut
{
    float4 mPosition : SV_POSITION;
    float4 mWorldPosition : WORLD_POSITION;
    float2 mTexcoord : TEXCOORD;
};

cbuffer GBufferConstants : register(b0)
{
    float4x4 gWVPMatrix;
    float4x4 gWorldMatrix;
    float gTextureTilingFactor;
    bool gHasDiffuseTexture;
};


GBufferVSOut vs_main(GBufferVSIn input)
{
    GBufferVSOut output;

    output.mPosition = mul(gWVPMatrix, float4(input.mPosition, 1.0f));
    output.mWorldPosition = mul(gWorldMatrix, float4(input.mPosition, 1.0f));
    output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}

#endif