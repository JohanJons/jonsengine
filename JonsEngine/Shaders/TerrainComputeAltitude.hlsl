#ifndef TERRAIN_COMPUTE_ALTITUDE_HLSL
#define TERRAIN_COMPUTE_ALTITUDE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<float> gAverageHeight : register( UAV_REGISTER_0 );

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	int2 texCoord = dispatchTID.xy;

    GroupMemoryBarrierWithGroupSync();

    uint texHeight = uint( gHeightmap[ texCoord ].r * 1000 );
    
    if ( texHeight == 0 )
        InterlockedAdd( gNumSeaSamples, 1 );
    else
        InterlockedAdd( gTotalHeightVals, texHeight );

    GroupMemoryBarrierWithGroupSync();

    uint numSamples = TERRAIN_NORMAL_THREADS_AXIS * TERRAIN_NORMAL_THREADS_AXIS;
    bool isOnlySea = gNumSeaSamples == numSamples;

    if ( groupIndex == 0 && !isOnlySea )
    {
        // only avg height for non-sea samples
        gTotalHeightVals /= ( numSamples - gNumSeaSamples );
        InterlockedAdd( gAverageHeight[ uint2(0, 0) ], gTotalHeightVals );
    }
}

#endif