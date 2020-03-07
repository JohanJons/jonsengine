#ifndef TERRAIN_PIXEL_DEBUG_NORMAL_HLSL
#define TERRAIN_PIXEL_DEBUG_NORMAL_HLSL

#define TERRAIN_DEBUG_NORMAL 1

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

struct PixelOut
{
	float3 mDiffuse : SV_Target0;
};


PixelOut ps_main( VertexOut input )
{
	PixelOut ret;
	ret.mDiffuse = input.mWorldNormal;

	return ret;
}

#endif