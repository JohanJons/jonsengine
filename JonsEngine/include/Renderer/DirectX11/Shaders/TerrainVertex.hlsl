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

// temp; refactor out
static const uint gridSize = 64;


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
	ret.mNormal = mul((float3x3)worldTransform, input.mNormal);	
	ret.mGridTexcoord = WorldXZToTexcoord(float2(gWorldMinExtentsX, gWorldMinExtentsZ), float2(gWorldMaxExtentsX, gWorldMaxExtentsZ), ret.mWorldPosition.xz);

	const uint mipLevel = 0;
	const float yOffset = gHeightmap.SampleLevel(gPointSampler, ret.mGridTexcoord, mipLevel).r;
	ret.mWorldPosition.y += gHeightModifier * yOffset;

	//ret.mWorldPosition = mul(viewProjection, ret.mWorldPosition);
	ret.mNormal = mul((float3x3)view, ret.mNormal);
	
	ret.mTessFactor = 1.0f;
	ret.mColor = input.mPosition;

	return ret;
}

#endif