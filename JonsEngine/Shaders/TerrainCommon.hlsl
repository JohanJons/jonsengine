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
	float gWorldMinX;
	float gWorldMinZ;
	float gWorldMaxX;
	float gWorldMaxZ;
	float gHeightModifier;
	float gVariationScale;
	uint gTransformOffset;
}


struct PatchTess
{
	float mEdgeTess[4] : SV_TessFactor;
	float mInsideTess[2] : SV_InsideTessFactor;
};

struct VertexOut
{
	float3 mWorldPosition : POSITION;
	float2 mTexcoord : TEXCOORD;
};

struct DomainOut
{
	float4 mPosition : SV_POSITION;
	float3 mNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
    float mTesellation : TESS_FACTOR;
	float mCoplanarity : COPLANARITY;
};

struct HullOut
{
	float3 mWorldPosition : POSITION;
	float2 mTexcoord : TEXCOORD;
};

#endif