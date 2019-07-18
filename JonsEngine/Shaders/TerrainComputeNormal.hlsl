#ifndef TERRAIN_COMPUTE_NORMAL_HLSL
#define TERRAIN_COMPUTE_NORMAL_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<float3> gNormalTexture : register( UAV_REGISTER );

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	float3 normal = SobelFilter( gHeightmap, int3( dispatchTID.xy, 0) );
	normal += 1.0f;
	normal *= 0.5f;

	gNormalTexture[ dispatchTID.xy ] = normal;
}

#endif