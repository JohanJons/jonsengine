#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float4 mPosition : POSITION;
	float2 mTexcoord : TEXCOORD;
	uint mInstanceID : SV_InstanceID;
};

StructuredBuffer<float4x4> gWorldTransforms : register (SBUFFER_REGISTER_EXTRA);

Texture2D gHeightmap : register(TEXTURE_REGISTER_EXTRA);
SamplerState gPointSampler : register(SAMPLER_REGISTER_POINT);

static const uint TRIANGLES_PER_PATCH = 2;

// temp; refactor out
static const uint gridSize = 64;


float2 WorldXZToTexcoord(float2 worldMinExtent, float2 worldMaxExtent, float2 worldPos)
{
	float2 terrainSize = worldMaxExtent - worldMinExtent;

	return worldPos / terrainSize;
}

VertexOut vs_main(VertexIn input)
{
	VertexOut ret;

	const uint transformIndex = ceil(input.mInstanceID + 1 / TRIANGLES_PER_PATCH) - 1;
	const float4x4 worldTransform = gWorldTransforms.Load(transformIndex);

	ret.mWorldPosition = mul(worldTransform, input.mPosition);
	ret.mGridTexcoord = WorldXZToTexcoord(gWorldMinExtents, gWorldMaxExtents, ret.mWorldPosition.xz);

	const uint mipLevel = 0;
	const float yOffset = gHeightmap.SampleLevel(gPointSampler, ret.mGridTexcoord, mipLevel).r;
	ret.mWorldPosition.y += yOffset;

	ret.mWorldPosition = mul(viewProjection, ret.mWorldPosition);

	//ret.mTexcoordLocal = input.mTexcoord;

	return ret;
}

#endif