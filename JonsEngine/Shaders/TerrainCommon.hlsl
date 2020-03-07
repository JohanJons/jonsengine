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
	float3 mNormal : NORMAL;
	uint mLOD : LOD;
	float mMorph : MORPH;
#ifdef TERRAIN_DEBUG_NORMAL
	float3 mWorldNormal : WORLD_NORMAL;
#endif
};

#endif