#ifndef TRANSFORM_ANIMATED_VERTEX_HLSL
#define TRANSFORM_ANIMATED_VERTEX_HLSL

#include "Constants.h"
#include "Common.hlsl"

struct TransformVSInput
{
	float4 mPosition : POSITION;
	uint4 mBoneIndices : BONE_INDICES;
	float4 mBoneWeights : BONE_WEIGHTS;
};

cbuffer TransformConstants : register(CBUFFER_REGISTER_VERTEX)
{
	float4x4 gWVPMatrix;
	uint gBoneIndexOffset;
};


float4 vs_main(TransformVSInput input) : SV_Position
{
	float4x4 boneTransform = BuildBoneTransform(input.mBoneIndices + gBoneIndexOffset, input.mBoneWeights);
	float4x4 wvpMatrix = mul(gWVPMatrix, boneTransform);

	return mul(wvpMatrix, input.mPosition);
}

#endif