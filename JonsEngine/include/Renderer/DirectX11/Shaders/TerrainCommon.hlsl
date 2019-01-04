#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

// TODO: move to common perframe CB
cbuffer PerFrameConstants : register(CBUFFER_REGISTER_DOMAIN)
{
	float gMinDistance;
	float gMaxDistance;
	float gMinFactor;
	float gMaxFactor;
}

cbuffer PerTerrainConstants : register(CBUFFER_REGISTER_EXTRA)
{
	float gHeightModifier;
	float gWorldMinX;
	float gWorldMinZ;
	float gWorldMaxX;
	float gWorldMaxZ;
}


struct PatchTess
{
	float mEdgeTess[4] : SV_TessFactor;
	float mInsideTess[2] : SV_InsideTessFactor;
};

struct VertexOut
{
	float4 mWorldPosition : POSITION;
	float3 mWorldNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
	float mTessFactor : TESS;
};

struct DomainOut
{
	float4 mPosition : SV_POSITION;
	float3 mNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
};

struct HullOut
{
	float4 mWorldPosition : POSITION;
	float3 mWorldNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
};

#endif