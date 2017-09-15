#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

// TODO: move to common perframe CB
cbuffer PerFrameConstants : register(CBUFFER_REGISTER_VERTEX)
{
	float gMinDistance;
	float gMaxDistance;
	float gMinFactor;
	float gMaxFactor;
}

cbuffer PerTerrainConstants : register(CBUFFER_REGISTER_EXTRA)
{
	float4x4 viewProjection;
	float2 gWorldMinExtents;
	float2 gWorldMaxExtents;
}


struct PatchTess
{
	float mEdgeTess[3] : SV_TessFactor;
	float mInsideTess : SV_InsideTessFactor;
};

struct VertexOut
{
	float4 mWorldPosition : SV_POSITION;
	float2 mGridTexcoord : TEXCOORD;
	float mTessFactor : TESS;
};

struct DomainOut
{
	float4 mPosition : SV_POSITION;
	float3 mPositionW : POSITION;
	float3 mNormalW : NORMAL;
	float2 mTexcoordW : TEXCOORD;
};

struct HullOut
{
	float3 mPositionW : POSITION;
	float3 mNormalW : NORMAL;
	float2 mTexcoordW : TEXCOORD;
};

#endif