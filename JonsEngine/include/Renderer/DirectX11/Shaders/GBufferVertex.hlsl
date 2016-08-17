#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

#include "Constants.h"
#include "Common.hlsl"

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

float4x4 BuildBoneTransform(const uint4 boneIndices, const float4 boneWeights);


GBufferVSOut vs_main(GBufferVSIn input)
{
    GBufferVSOut output = (GBufferVSOut)0;

	output.mPosition = input.mPosition;
	output.mNormal = input.mNormal;
	output.mTangent = input.mTangent;
	output.mBitangent = input.mBitangent;

	float4x4 wvpMatrix = gWVPMatrix;
	float4x4 worldViewMatrix = gWorldViewMatrix;
    // temporary solution
    if (gIsAnimating)
    {
		float4x4 boneTransform = BuildBoneTransform(input.mBoneIndices, input.mBoneWeights);
		wvpMatrix = mul(wvpMatrix, boneTransform);
		worldViewMatrix = mul(worldViewMatrix, boneTransform);
    }
        
    output.mPosition = mul(wvpMatrix, output.mPosition);

    // rotating/translation/uniform scaling is fine, we dont need to normalize in VS
    output.mNormal = mul((float3x3)worldViewMatrix, output.mNormal);
    output.mTangent = mul((float3x3)worldViewMatrix, output.mTangent);
    output.mBitangent = mul((float3x3)worldViewMatrix, output.mBitangent);

    output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}


// assumes there will at most be four bones
float4x4 BuildBoneTransform(const uint4 boneIndices, const float4 boneWeights)
{
	uint boneIndex = boneIndices[0];
	float boneWeight = boneWeights[0];
	float4x4 boneTransform = gBones[boneIndex] * boneWeight;

	boneWeight = boneWeights[1];
	if (boneWeight > 0.0)
	{
		uint boneIndex = boneIndices[1];
		boneTransform += gBones[boneIndex] * boneWeight;

		boneWeight = boneWeights[2];
		if (boneWeight > 0.0)
		{
			uint boneIndex = boneIndices[2];
			boneTransform += gBones[boneIndex] * boneWeight;

			boneWeight = boneWeights[3];
			if (boneWeight > 0.0)
			{
				uint boneIndex = boneIndices[3];
				boneTransform += gBones[boneIndex] * boneWeight;
			}
		}
	}

	return boneTransform;
}

#endif