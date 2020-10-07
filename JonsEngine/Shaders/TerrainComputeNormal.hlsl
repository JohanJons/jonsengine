#ifndef TERRAIN_COMPUTE_NORMAL_HLSL
#define TERRAIN_COMPUTE_NORMAL_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<float3> gNormalTexture : register( UAV_REGISTER_0 );
RWTexture2D<uint> gTopography : register( UAV_REGISTER_1 );

uint CalculateTopography( in Texture2D normalTexture, int3 texCoord )
{
	float h00 = normalTexture.Load( texCoord ).r;

	return GetTopography( h00 );
}

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	float3 normal = SobelFilter( gHeightmap, int3( dispatchTID.xy, 0) );
	normal += 1.0f;
	normal *= 0.5f;

	gNormalTexture[ dispatchTID.xy ] = normal;
	gTopography[ dispatchTID.xy ] = CalculateTopography( gHeightmap, int3( dispatchTID.xy, 0) );
}

#endif