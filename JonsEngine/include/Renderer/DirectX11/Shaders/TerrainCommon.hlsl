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
	float gWorldMinExtentsX;
	float gWorldMinExtentsZ;
	float gWorldMaxExtentsX;
	float gWorldMaxExtentsZ;
}


struct PatchTess
{
	float mEdgeTess[4] : SV_TessFactor;
	float mInsideTess[2] : SV_InsideTessFactor;
};

struct VertexOut
{
	float4 mWorldPosition : POSITION;
	float mTessFactor : TESS;
};

struct DomainOut
{
	float4 mPosition : SV_POSITION;
	float3 mNormal : NORMAL;
};

struct HullOut
{
	float4 mWorldPosition : POSITION;
};

#endif