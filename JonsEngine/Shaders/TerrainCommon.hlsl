#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

Texture2D gCoplanarityMap : register( TEXTURE_REGISTER_EXTRA_2 );

cbuffer PerTerrainConstants : register(CBUFFER_REGISTER_DOMAIN)
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
	//uint2 extent = uint2( gWorldMaxX - gWorldMinX, gWorldMaxZ - gWorldMinZ );
	uint2 asd = float2( gWorldMaxX, gWorldMaxZ ) - worldPatchMidpoint;
	asd /= gTerrainPatchSize;
	//uint2 offset = uint2( worldPatchMidpoint.xy / uint2( gWorldMaxX, gWorldMaxZ ) );

	return gCoplanarityMap.Load( uint3( asd, 0 ) ).r;

	/*ret.mTexcoord = 1.0f / ( float2(gWorldMaxX, gWorldMaxZ) + ( -float2(gWorldMinX, gWorldMinZ) ) );
	ret.mTexcoord = ( -float2( gWorldMinX, gWorldMinZ ) + ret.mWorldPosition.xz ) * ret.mTexcoord;
	ret.mTexcoord = clamp( ret.mTexcoord, 0.0f, 1.0f );*/
}

#endif