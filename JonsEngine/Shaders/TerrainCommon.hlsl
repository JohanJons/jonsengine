#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#if defined( TERRAIN_NORMAL_SIMPLE ) || defined ( TERRAIN_NORMAL_BETTER )
	#define TERRAIN_VERTEX_NORMALS 1
#endif

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
#ifdef TERRAIN_VERTEX_NORMALS
	float3 mNormal : NORMAL;
	#ifdef TERRAIN_DEBUG_NORMAL
		float3 mWorldNormal : NORMAL_WORLD;
	#endif
#endif
	uint mLOD : LOD;
	float mMorph : MORPH;
};

#endif