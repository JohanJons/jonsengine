#ifndef TERRAIN_COMPUTE_MOISTURE_HLSL
#define TERRAIN_COMPUTE_MOISTURE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
Texture2D<uint2> gJumpFloodRiverMap : register( TEXTURE_REGISTER_EXTRA_2 );
RWTexture2D<float> gMoistureMap : register( UAV_REGISTER_0 );
RWTexture2D<uint> gAverageHeight : register( UAV_REGISTER_1 );

cbuffer MoistureCBuffer : register( CBUFFER_REGISTER_EXTRA )
{
	float gFalloffBegin;
	float gFalloffDistance;
	float gHeightScale;
}

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	int2 texCoord = dispatchTID.xy;
	int2 closestRiverTexcoord = gJumpFloodRiverMap[ texCoord ];
	float dist = distance( (float2)closestRiverTexcoord, (float2)texCoord );

	float texHeight = gHeightmap[ texCoord ].r;
	float riverHeight = gHeightmap[ closestRiverTexcoord ].r;
	float heightDiff = texHeight - riverHeight;
	float higherElevationDiff = max( heightDiff, 0.0f );

	if ( dist < gFalloffBegin  )
	{
		gMoistureMap[ texCoord ] = 1.0;
		return;
	}

	float distanceBeyondBeginFalloff = dist - gFalloffBegin;
	float moistureLerp = smoothstep( 0, gFalloffDistance, distanceBeyondBeginFalloff );

	float moisture = 1.0 - moistureLerp;
	moisture -= 20 * pow( higherElevationDiff, 2.0f );
	moisture = clamp( moisture, 0.0, 1.0 );

	gMoistureMap[ texCoord ] = moisture;
}

#endif