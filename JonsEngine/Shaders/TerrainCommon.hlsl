#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

Texture2D gCoplanarityMap : register( TEXTURE_REGISTER_EXTRA_2 );
Texture2D gNormalMap : register( TEXTURE_REGISTER_NORMAL );

cbuffer PerQuadConstants : register( CBUFFER_REGISTER_VERTEX )
{
	float4x4 gQuadWorldTransform;
	uint gLODLevel;
}

cbuffer PerTerrainConstants : register(CBUFFER_REGISTER_DOMAIN)
{
	float2 gWorldMin;
	float2 gWorldMax;
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
	float4 mPosition : SV_POSITION;
	//float2 mTexcoord : TEXCOORD;
};

struct DomainOut
{
	float4 mPosition : SV_POSITION;
	float3 mNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
    float mTesellation : TESS_FACTOR;
#ifdef TERRAIN_DEBUG_COPLANARITY
	float mCoplanarity : COPLANARITY;
#endif
};

struct HullOut
{
	float3 mWorldPosition : POSITION;
	float2 mTexcoord : TEXCOORD;
};

float GetCoplanarity( float2 worldPatchMidpoint )
{
	uint2 uv = ( worldPatchMidpoint - gWorldMin ) / gTerrainCoplanaritySize;
	return gCoplanarityMap.Load( uint3( uv, 0 ) ).r;
}

float3 GetNormal( float2 worldPatchMidpoint )
{
	uint2 uv = ( worldPatchMidpoint - gWorldMin ) / gTerrainCoplanaritySize;
	return gNormalMap.Load( uint3( uv, 0 ) ).rgb;
}

#endif