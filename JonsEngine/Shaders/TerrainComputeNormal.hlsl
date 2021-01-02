#ifndef TERRAIN_COMPUTE_NORMAL_HLSL
#define TERRAIN_COMPUTE_NORMAL_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
Texture2D gRivermap : register( TEXTURE_REGISTER_EXTRA_2 );
RWTexture2D<float3> gNormalTexture : register( UAV_REGISTER_0 );
RWTexture2D<uint2> gJumpFloodRiverMap : register( UAV_REGISTER_1 );

bool PopulateRivers( int3 texCoord )
{
	float3 riverData = gRivermap.Load( texCoord ).rgb;
	if ( IsEqual( riverData, COLOR_BLACK ) )
		return false;

	gJumpFloodRiverMap[ texCoord.xy ] = texCoord.xy;
	return true;
}

/*void CalculateTopography( int3 texCoord )
{
	float h00 = gHeightmap.Load( texCoord ).r;
	if ( h00 <= 0.0f )
	{
		gTopography[ texCoord.xy ] = uint3( TOPOGRAPHY_SEA, 0, 0 );
	}
	else if ( h00 > 0.0f && h00 <= 0.1f )
	{
		float h10 = gHeightmap.Load( texCoord, int2( -1, 0 ) ).r;
		float h01 = gHeightmap.Load( texCoord, int2( 1, 0 ) ).r;
		float h11 = gHeightmap.Load( texCoord, int2( 0, 1 ) ).r;
		float h101 = gHeightmap.Load( texCoord, int2( 0, -1 ) ).r;

		gTopography[ texCoord.xy ] = uint3( TOPOGRAPHY_PLAINS, 0, 0 );
	}
	else if ( h00 > 0.1f && h00 < 0.4f )
	{
		gTopography[ texCoord.xy ] = uint3( TOPOGRAPHY_PLAINS, 0, 0 );
	}
	else
	{
		gTopography[ texCoord.xy ] = uint3( TOPOGRAPHY_MOUNTAINS, 0, 0 );
	}
}*/

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	int3 texCoord = int3( dispatchTID.xy, 0 );

	float3 normal = SobelFilter( gHeightmap, texCoord );
	normal += 1.0f;
	normal *= 0.5f;

	gNormalTexture[ dispatchTID.xy ] = normal;

	PopulateRivers( texCoord );
}

#endif