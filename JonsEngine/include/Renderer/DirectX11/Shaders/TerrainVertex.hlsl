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

	return ret;
}

#endif