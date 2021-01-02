#ifndef TERRAIN_COMPUTE_JFA_HLSL
#define TERRAIN_COMPUTE_JFA_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

cbuffer PerTerrainConstants : register( CBUFFER_REGISTER_EXTRA )
{
	int2 gSampleOffset;
}

RWTexture2D<uint2> gJumpFloodRiverMap : register( UAV_REGISTER_0 );

void UpdateClosest( inout int2 currentValue, int2 currentTexcoord, int2 jumpTexcoord )
{
	int2 jumpValue = gJumpFloodRiverMap[ jumpTexcoord ];
	if ( IsZero( jumpValue ) )
	{
		return;
	}

	float jumpDistance = distance( (float2)currentTexcoord, (float2)jumpValue );
	if ( IsZero( currentValue ) )
	{
		gJumpFloodRiverMap[ currentTexcoord ] = jumpValue;
		currentValue = jumpValue;
		return;
	}

	float currentDistance = distance( (float2)currentTexcoord, (float2)currentValue );
	if ( jumpDistance < currentDistance )
	{
		gJumpFloodRiverMap[ currentTexcoord ] = jumpValue;
		currentValue = jumpValue;
	}
}

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	int2 texCoord = dispatchTID.xy;
	int2 currentValue = gJumpFloodRiverMap[ texCoord ];

	int2 texCoordT = texCoord + int2( 0, gSampleOffset.y );
	int2 texCoordTR = texCoord + gSampleOffset;
	int2 texCoordR = texCoord + int2( gSampleOffset.x, 0 );
	int2 texCoordBR = texCoord + int2( gSampleOffset.x, -gSampleOffset.y );
	int2 texCoordB = texCoord + int2( 0, -gSampleOffset.y );
	int2 texCoordBL = texCoord - gSampleOffset;
	int2 texCoordL = texCoord + int2( -gSampleOffset.x, 0 );
	int2 texCoordTL = texCoord + int2( -gSampleOffset.x, gSampleOffset.y );

	UpdateClosest( currentValue, texCoord, texCoordT );
	UpdateClosest( currentValue, texCoord, texCoordTR );
	UpdateClosest( currentValue, texCoord, texCoordR );
	UpdateClosest( currentValue, texCoord, texCoordBR );
	UpdateClosest( currentValue, texCoord, texCoordB );
	UpdateClosest( currentValue, texCoord, texCoordBL );
	UpdateClosest( currentValue, texCoord, texCoordL );
	UpdateClosest( currentValue, texCoord, texCoordTL );
}

#endif