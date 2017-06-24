#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float4 mPosition : POSITION;
	float3 mNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
};


VertexOut vs_main(VertexIn input)
{
	VertexOut ret;

	ret.mPositionW = mul(gWorldTransform, input.mPosition);
	ret.mNormalW = mul(gWorldTransform, input.mNormal);
	ret.mTexcoordW = mul(gWorldTransform, input.mTexcoord);

	return ret;
}

#endif