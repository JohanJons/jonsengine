#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

Texture2D gNormalMap : register( TEXTURE_REGISTER_NORMAL );

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

#endif