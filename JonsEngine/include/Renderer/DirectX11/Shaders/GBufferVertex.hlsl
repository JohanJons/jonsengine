#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

#include "Constants.h"

struct GBufferVSIn
{
    float4 mPosition : POSITION;
    float3 mNormal : NORMAL;
    float2 mTexcoord : TEXCOORD;
    float3 mTangent : TANGENT;
    float3 mBitangent : BITANGENT;
    uint4 mBoneIndices : BONE_INDICES;
    float4 mBoneWeights : BONE_WEIGHTS;
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
    bool gIsAnimating;
};

StructuredBuffer<float4x4> gBones : register (SBUFFER_REGISTER_VERTEX);

float3 ApplyBone(float3 normal, uint4 boneIndices, float4 boneWeights, uint boneNum);
float4 ApplyBone(float4 position, uint4 boneIndices, float4 boneWeights, uint boneNum);


GBufferVSOut vs_main(GBufferVSIn input)
{
    GBufferVSOut output = (GBufferVSOut)0;

    // temporary solution
    if (gIsAnimating)
    {
        for (uint boneNum = 0; boneNum < MAX_BONES_PER_VERTEX; ++boneNum)
        {
            output.mPosition += ApplyBone(input.mPosition, input.mBoneIndices, input.mBoneWeights, boneNum);
            output.mNormal += ApplyBone(input.mNormal, input.mBoneIndices, input.mBoneWeights, boneNum);

            if (gHasNormalTexture)
            {
                output.mTangent += ApplyBone(input.mTangent, input.mBoneIndices, input.mBoneWeights, boneNum);
                output.mBitangent += ApplyBone(input.mBitangent, input.mBoneIndices, input.mBoneWeights, boneNum);
            }
        }
    }
    else
    {
        output.mPosition = input.mPosition;
        output.mNormal = input.mNormal;
        output.mTangent = input.mTangent;
        output.mBitangent = input.mBitangent;
    }
        
    output.mPosition = mul(gWVPMatrix, output.mPosition);

    // rotating/translation/uniform scaling is fine, we dont need to normalize in VS
    output.mNormal = mul((float3x3)gWorldViewMatrix, output.mNormal);
    output.mTangent = mul((float3x3)gWorldViewMatrix, output.mTangent);
    output.mBitangent = mul((float3x3)gWorldViewMatrix, output.mBitangent);

    output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}


float3 ApplyBone(float3 normal, uint4 boneIndices, float4 boneWeights, uint boneNum)
{
    return ApplyBone(float4(normal, 0.0), boneIndices, boneWeights, boneNum).xyz;
}

float4 ApplyBone(float4 position, uint4 boneIndices, float4 boneWeights, uint boneNum)
{
    const uint boneIndex = boneIndices[boneNum];
    const uint boneWeight = boneWeights[boneNum];

    return mul(position, gBones[boneIndex]) * boneWeight;
}

#endif