#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

#define TOPOGRAPHY_RIVER 0
#define TOPOGRAPHY_SEA 1
#define TOPOGRAPHY_PLAINS 2
#define TOPOGRAPHY_MOUNTAINS 3
#define TOPOGRAPHY_COUNT 4

Texture2D gNormalMap : register( TEXTURE_REGISTER_NORMAL );
Texture2D gTopographyMap : register( TEXTURE_REGISTER_EXTRA_2 );

cbuffer PerTerrainConstants : register( CBUFFER_REGISTER_EXTRA )
{
	float2 gWorldMin;
	float2 gWorldMax;
	float gHeightModifier;
	float gVariationScale;
	uint gTransformOffset;
}

struct VertexOut
{
	float4 mPosition : SV_POSITION;
	float2 mTexcoord : TEXCOORD;
	uint mLOD : LOD;
	float mMorph : MORPH;
};

uint GetTopography( float heightVal )
{
	if ( heightVal <= 0.0f )
	{
		return TOPOGRAPHY_SEA;
	}
	else if ( heightVal > 0.0f && heightVal <= 0.1f )
	{
		return TOPOGRAPHY_RIVER;
	}
	else if ( heightVal > 0.1f && heightVal < 0.4f )
	{
		return TOPOGRAPHY_PLAINS;
	}
	else
	{
		return TOPOGRAPHY_MOUNTAINS;
	}
}

#endif