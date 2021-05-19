#ifndef TERRAIN_COMPUTE_AVERAGE_ALTITUDE_FINAL_HLSL
#define TERRAIN_COMPUTE_AVERAGE_ALTITUDE_FINAL_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

cbuffer DispatchDimensions : register( CBUFFER_REGISTER_EXTRA )
{
    uint gDispatchX;
    uint gDispatchY;
}

RWTexture2D<uint> gAverageHeight : register( UAV_REGISTER_0 );

[numthreads(1, 1, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
    uint numAveragedSamples = gDispatchX * gDispatchY;
    uint avgHeightValue = gAverageHeight[ uint2( 0, 0 ) ];
    avgHeightValue /= numAveragedSamples;
    gAverageHeight[ uint2( 0, 0 ) ] = avgHeightValue;
}

#endif