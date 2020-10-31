#ifndef TERRAIN_PIXEL_TOPOGRAPHY_DEBUG_HLSL
#define TERRAIN_PIXEL_TOPOGRAPHY_DEBUG_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
};

static const float3 DebugTopographyColors[ TOPOGRAPHY_COUNT ] = { COLOR_CYAN, COLOR_BLUE, COLOR_ORANGE, COLOR_GREY };

PixelOut ps_main( VertexOut input )
{
	PixelOut ret;

	int2 index = input.mTexcoord * int2(12000, 9000);

	uint3 topographyIndex = gTopographyMap.Load( int3( index, 0 ) );
	float3 debugColor = DebugTopographyColors[ topographyIndex.x ];
	ret.mDiffuse = float4( debugColor, 1.0f );

	return ret;
}

#endif