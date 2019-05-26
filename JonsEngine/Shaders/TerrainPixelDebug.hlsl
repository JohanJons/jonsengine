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

    float tess = input.mTesellation / 64;
    if ( input.mTesellation < 32 )
    {
        ret.mDiffuse = lerp( RED, GREEN, tess );
    }
    else
    {
        ret.mDiffuse = lerp( GREEN, BLUE, tess );
    }

	return ret;
}

#endif