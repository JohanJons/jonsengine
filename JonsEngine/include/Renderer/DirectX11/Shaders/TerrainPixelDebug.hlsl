#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
};


PixelOut ps_main(DomainOut input)
{
	PixelOut ret;

	ret.mDiffuse = float4( 1.0f, 0.0f, 0.0f, 1.0f);

	return ret;
}

#endif