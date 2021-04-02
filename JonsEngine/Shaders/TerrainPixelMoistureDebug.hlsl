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

	int2 index = input.mTexcoord * ( int2( gWorldMax - gWorldMin ) ) ;

	float moisture = gMoistureMap.Load( int3( index, 0 ) ).x;
	float4 moistureColor = float4( lerp( COLOR_GREY, COLOR_GREEN, moisture ), 1.0 );

	ret.mDiffuse = moistureColor;

	return ret;
}

#endif