#ifndef TERRAIN_COMPUTE_ALTITUDE_HLSL
#define TERRAIN_COMPUTE_ALTITUDE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

groupshared uint gTotalHeightVals = 0;

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<uint> gAverageHeight : register( UAV_REGISTER_0 );

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	int3 texCoord = int3( dispatchTID.xy, 0 );

    GroupMemoryBarrierWithGroupSync();

    InterlockedAdd( gTotalHeightVals, 1 );
    //gTotalHeightVals += 2;

    GroupMemoryBarrierWithGroupSync();
    GroupMemoryBarrierWithGroupSync();

    if ( groupIndex == 0 )
    {
        InterlockedAdd( gAverageHeight[ uint2(0, 0) ], gTotalHeightVals );
        //uint oldval = gAverageHeight[ uint2( 0, 0 ) ];
        //gAverageHeight[ uint2(0, 0) ] += gTotalHeightVals;
        //gAverageHeight[ dispatchTID.xy ] += gTotalHeightVals;
        //gAverageHeight[uint2(0, 0)] = uint(5);
    }
    GroupMemoryBarrierWithGroupSync();
}

#endif