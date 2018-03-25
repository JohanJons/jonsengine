#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float4 mPosition : POSITION;
	float3 mNormal : NORMAL;
	uint mInstanceID : SV_InstanceID;
};

StructuredBuffer<float4x4> gWorldTransforms : register (SBUFFER_REGISTER_EXTRA);

Texture2D gHeightmap : register(TEXTURE_REGISTER_EXTRA);
SamplerState gPointSampler : register(SAMPLER_REGISTER_POINT);


float2 WorldXZToTexcoord(float2 worldMinExtent, float2 worldMaxExtent, float2 worldPos)
{
	return abs((worldPos - worldMinExtent) / (worldMaxExtent - worldMinExtent));
}

VertexOut vs_main(VertexIn input)
{
	VertexOut ret;

	const uint transformIndex = input.mInstanceID;
	const float4x4 worldTransform = gWorldTransforms.Load(transformIndex);

	ret.mWorldPosition = mul(worldTransform, input.mPosition);
	ret.mTessFactor = 2.0f;

	return ret;
}

#endif