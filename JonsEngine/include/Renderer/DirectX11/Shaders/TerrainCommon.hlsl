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
	float4x4 viewProjection;
	float4x4 view;
	float gHeightModifier;
	float gWorldMinExtentsX;
	float gWorldMinExtentsZ;
	float gWorldMaxExtentsX;
	float gWorldMaxExtentsZ;
}


struct PatchTess
{
	float mEdgeTess[3] : SV_TessFactor;
	float mInsideTess : SV_InsideTessFactor;
};

struct VertexOut
{
	float4 mWorldPosition : POSITION;
	//float3 mNormal : NORMAL;
	//float2 mGridTexcoord : TEXCOORD;
	//float4 mColor : COLOR;
	float mTessFactor : TESS;
};

struct DomainOut
{
	float4 mWorldPosition : SV_POSITION;
};

struct HullOut
{
	float4 mWorldPosition : POSITION;
	//float3 mNormalW : NORMAL;
	//float2 mTexcoordW : TEXCOORD;
};

#endif