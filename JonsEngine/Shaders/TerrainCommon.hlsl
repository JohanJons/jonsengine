#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

#include "Common.hlsl"

#define TOPOGRAPHY_RIVER 0
#define TOPOGRAPHY_SEA 1
#define TOPOGRAPHY_PLAINS 2
#define TOPOGRAPHY_MOUNTAINS 3
#define TOPOGRAPHY_COUNT 4

Texture2D gNormalMap : register( TEXTURE_REGISTER_NORMAL );
Texture2D<uint3> gTopographyMap : register( TEXTURE_REGISTER_EXTRA_2 );

struct VertexOut
{
	float4 mPosition : SV_POSITION;
	float2 mTexcoord : TEXCOORD;
	uint mLOD : LOD;
	float mMorph : MORPH;
};

#endif