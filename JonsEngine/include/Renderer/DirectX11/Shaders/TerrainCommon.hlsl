#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

cbuffer VertexConstants : register(CBUFFER_REGISTER_VERTEX)
{
	float4x4 gWorldTransform;
	float mMinDistance;
	float mMaxDistance;
	float mMinFactor;
	float mMaxFactor;
}


struct PatchTess
{
	float mEdgeTess[3] : SV_TessFactor;
	float mInsideTess : SV_InsideTessFactor;
};

struct VertexOut
{
	float3 mPositionW : POSITION;
	float3 mNormalW : NORMAL;
	float2 mTexcoordW : TEXCOORD;
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