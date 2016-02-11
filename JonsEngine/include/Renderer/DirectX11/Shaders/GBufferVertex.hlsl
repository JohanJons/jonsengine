#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

#include "Constants.h"

struct GBufferVSIn
{
    float3 mPosition : POSITION;
    float3 mNormal : NORMAL;
    float2 mTexcoord : TEXCOORD;
    float3 mTangent : TANGENT;
    float3 mBitangent : BITANGENT;
    float mBoneIndices[MAX_BONES] : BONE_INDICES;
    float mBoneWeights[MAX_BONES] : BONE_WEIGHTS;
};

struct GBufferVSOut
{
    float4 mPosition : SV_POSITION;
    float3 mNormal : NORMAL;
    float2 mTexcoord : TEXCOORD;
    float3 mTangent : TANGENT;
    float3 mBitangent : BITANGENT;
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
    bool gHasBones;
};


GBufferVSOut vs_main(GBufferVSIn input)
{
    GBufferVSOut output;

    output.mPosition = mul(gWVPMatrix, float4(input.mPosition, 1.0));

    // scaling/translation/uniform scaling is fine, we dont need to normalize in VS
    output.mNormal = mul((float3x3)gWorldViewMatrix, input.mNormal);
    output.mTangent = mul((float3x3)gWorldViewMatrix, input.mTangent);
    output.mBitangent = mul((float3x3)gWorldViewMatrix, input.mBitangent);

    output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}

#endif