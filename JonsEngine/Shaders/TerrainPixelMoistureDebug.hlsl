#ifndef TERRAIN_PIXEL_MOISTURE_DEBUG_HLSL
#define TERRAIN_PIXEL_MOISTURE_DEBUG_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

Texture2D<float> gMoistureMap : register( TEXTURE_REGISTER_EXTRA_3 );

cbuffer PerTerrainConstants : register( CBUFFER_REGISTER_EXTRA )
{
	float2 gWorldMin;
	float2 gWorldMax;
	float gHeightModifier;
	float gVariationScale;
	uint gTransformOffset;
}

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
};

PixelOut ps_main( VertexOut input )
{
	PixelOut ret;

	int2 index = input.mTexcoord * int2(12000, 9000);

	float dist = gMoistureMap.Load( int3( index, 0 ) ).x;
	float moisture = 1 - dist / ( distance( gWorldMin, gWorldMax ) );

	ret.mDiffuse = float4( moisture, moisture, moisture, 1.0f );

	return ret;
}

#endif