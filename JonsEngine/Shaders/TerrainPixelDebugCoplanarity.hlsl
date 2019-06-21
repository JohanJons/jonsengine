#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

struct PixelOut
{
	float3 mDiffuse : SV_Target0;
};


PixelOut ps_main( DomainOut input )
{
	PixelOut ret;

    if ( input.mCoplanarity < 0.5f )
    {
		ret.mDiffuse = lerp( RED, GREEN, input.mCoplanarity );
    }
    else
    {
        ret.mDiffuse = lerp( GREEN, BLUE, input.mCoplanarity );
    }

	return ret;
}

#endif