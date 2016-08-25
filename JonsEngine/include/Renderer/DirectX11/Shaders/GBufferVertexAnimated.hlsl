#ifndef GBUFFER_VERTEX_ANIMATED_HLSL
#define GBUFFER_VERTEX_ANIMATED_HLSL

#define GBUFFER_IS_ANIMATED_MESHES 1

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

#include "GbufferVertex.hlsl"

#endif