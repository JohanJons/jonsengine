#ifndef TERRAIN_PIXEL_DEBUG_COPLANARITY_HLSL
#define TERRAIN_PIXEL_DEBUG_COPLANARITY_HLSL

#define TERRAIN_DEBUG_COPLANARITY 1

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

struct PixelOut
{
	float3 mDiffuse : SV_Target0;
};


PixelOut ps_main( DomainOut input )
{
	PixelOut ret;

	ret.mDiffuse = lerp( COLOR_BLACK, COLOR_RED, input.mCoplanarity );

	return ret;
}

#endif