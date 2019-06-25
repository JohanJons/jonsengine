#ifndef COMMON_HLSL
#define COMMON_HLSL

#include "Constants.hlsl"

static const float3 RED = float3( 1.0, 0.0, 0.0 );
static const float3 GREEN = float3( 0.0, 1.0, 0.0 );
static const float3 BLUE = float3( 0.0, 0.0, 1.0 );
static const float3 BLACK = float3( 0.0, 0.0, 0.0 );
static const float3 WHITE = float3( 1.0, 1.0, 1.0 );

StructuredBuffer<float4x4> gBones : register (SBUFFER_REGISTER_BONE_TRANSFORMS);

SamplerState gPointSampler : register(SAMPLER_REGISTER_POINT);
SamplerState gLinearSampler : register(SAMPLER_REGISTER_LINEAR);
SamplerState gLinearWrapSampler : register( SAMPLER_REGISTER_LINEAR_WRAP );
SamplerState gAnisotropicSampler : register(SAMPLER_REGISTER_ANISOTROPIC);

cbuffer PerFrameCB : register(CBUFFER_REGISTER_PER_FRAME)
{
	float4x4 gFrameViewProj;
	float4x4 gFrameView;
	float4x4 gFrameInvView;
	float4x4 gFrameInvProj;
	float3 gWorldEyePos;
	float gMinZ;
	float gMaxZ;
	uint gTerrainPatchSize;
};


float3 SampleNormalTexture(in Texture2D normalTexture, uint2 samplePosition)
{
    float3 normal = normalTexture[samplePosition].xyz;

    normal *= 2.0;
    normal -= 1.0;

    return normal;
}

float3 ReconstructViewPosition(const float depth, const float2 texCoord, const float4x4 invProjMatrix)
{
    float x = texCoord.x * 2 - 1;
    float y = (1 - texCoord.y) * 2 - 1;
    float4 projPos = float4(x, y, depth, 1.0);

    float4 viewPos = mul(invProjMatrix, projPos);

    return viewPos.xyz /= viewPos.w;
}

float3x3 CreateMatrixFromCols(const float3 c0, const float3 c1, const float3 c2)
{
    return float3x3(c0.x, c1.x, c2.x,
                    c0.y, c1.y, c2.y,
                    c0.z, c1.z, c2.z);
}

float4x4 CreateMatrixFromCols(const float4 c0, const float4 c1, const float4 c2, const float4 c3)
{
    return float4x4(c0.x, c1.x, c2.x, c3.x,
                    c0.y, c1.y, c2.y, c3.y,
                    c0.z, c1.z, c2.z, c3.z,
                    c0.w, c1.w, c2.w, c3.w);
}

float4x4 BuildBoneTransform(const uint4 boneIndices, const float4 boneWeights)
{
	uint boneIndex = boneIndices[0];
	float boneWeight = boneWeights[0];
	float4x4 boneTransform = gBones[boneIndex] * boneWeight;

	for (uint boneNum = 1; boneNum < NUM_BONES_PER_VERTEX; ++boneNum)
	{
		boneIndex = boneIndices[boneNum];
		boneWeight = boneWeights[boneNum];

		boneTransform += gBones[boneIndex] * boneWeight;
	}

	return boneTransform;
}

float CalcLuminance(const float3 color)
{
	return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
}


#endif