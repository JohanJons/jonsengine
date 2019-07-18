#ifndef TERRAIN_PIXEL_DEBUG_NORMAL_HLSL
#define TERRAIN_PIXEL_DEBUG_NORMAL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

struct PixelOut
{
	float3 mDiffuse : SV_Target0;
};


PixelOut ps_main( DomainOut input )
{
	PixelOut ret;

	/*float x = lerp( 0.0f, 1.0f, input.mTexcoord.x );
	float y = lerp( 0.0f, 1.0f, input.mTexcoord.y );
	ret.mDiffuse = float3( x, y, 0.0f );*/

	ret.mDiffuse = input.mNormal;

	return ret;
}

#endif