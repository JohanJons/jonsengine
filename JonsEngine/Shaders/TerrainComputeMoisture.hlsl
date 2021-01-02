#ifndef TERRAIN_COMPUTE_MOISTURE_HLSL
#define TERRAIN_COMPUTE_MOISTURE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

Texture2D<uint2> gJumpFloodRiverMap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<float> gMoistureMap : register( UAV_REGISTER_0 );

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	int2 texCoord = dispatchTID.xy;
	int2 currentValue = gJumpFloodRiverMap[ texCoord ];

	float dist = distance( (float2)currentValue, (float2)texCoord );
	gMoistureMap[ texCoord ] = dist;
}

#endif