#ifndef TERRAIN_COMPUTE_HLSL
#define TERRAIN_COMPUTE_HLSL

#include "Constants.hlsl"

#define TERRAIN_COMPUTE_NUM_THREADS 16

RWTexture2D<unorm float> gCoplanarityTexture : register(UAV_REGISTER);

groupshared float2 gDepthSamples[ TERRAIN_COMPUTE_PATCH_EXTENT * TERRAIN_COMPUTE_PATCH_EXTENT ];
groupshared float4 gPlane;

[numthreads(TERRAIN_COMPUTE_NUM_THREADS, TERRAIN_COMPUTE_NUM_THREADS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint threadIndex : SV_GroupIndex)
{
    uint texelsPerThread = TERRAIN_COMPUTE_PATCH_EXTENT / TERRAIN_COMPUTE_NUM_THREADS;

}

#endif