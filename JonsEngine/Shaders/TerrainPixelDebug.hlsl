#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

struct PixelOut
{
	float3 mDiffuse : SV_Target0;
};

static const uint gNumDebugColors = 3;
static const float3 DebugLODColors[ gNumDebugColors ] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };

PixelOut ps_main( VertexOut input )
{
	PixelOut ret;

	float3 currLod = DebugLODColors[ input.mLOD % gNumDebugColors ];
	float3 nextLod = DebugLODColors[ ( input.mLOD - 1 ) % gNumDebugColors ];
	ret.mDiffuse = lerp( currLod, nextLod, input.mMorph );

	return ret;
}

#endif