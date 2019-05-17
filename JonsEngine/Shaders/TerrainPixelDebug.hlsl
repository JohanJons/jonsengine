#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
};


PixelOut ps_main( DomainOut input )
{
	PixelOut ret;

    if ( input.mTesellation < 8 )
    {
        ret.mDiffuse = float4( 1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if ( input.mTesellation >= 8 && input.mTesellation < 16 )
    {
        ret.mDiffuse = float4( 0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if ( input.mTesellation >= 16 && input.mTesellation < 24 )
    {
        ret.mDiffuse = float4( 0.0f, 0.0f, 1.0f, 1.0f);
    }
    else if ( input.mTesellation >= 24 && input.mTesellation < 32 )
    {
        ret.mDiffuse = float4( 1.0f, 0.0f, 1.0f, 1.0f);
    }
    else if ( input.mTesellation >= 32 && input.mTesellation < 40 )
    {
        ret.mDiffuse = float4( 1.0f, 1.0f, 0.0f, 1.0f);
    }
    else if ( input.mTesellation >= 40 && input.mTesellation < 48 )
    {
        ret.mDiffuse = float4( 0.0f, 1.0f, 1.0f, 1.0f);
    }
    else if ( input.mTesellation >= 48 && input.mTesellation < 56 )
    {
        ret.mDiffuse = float4( 1.0f, 1.0f, 0.5f, 1.0f);
    }
    else if ( input.mTesellation >= 56 )
    {
        ret.mDiffuse = float4( 0.5f, 0.0f, 1.0f, 1.0f);
    }

	return ret;
}

#endif