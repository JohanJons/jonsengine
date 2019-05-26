#ifndef TERRAIN_COMPUTE_HLSL
#define TERRAIN_COMPUTE_HLSL

#include "Constants.hlsl"

#define TERRAIN_COMPUTE_NUM_THREADS 16

RWTexture2D<unorm float> gCoplanarityTexture : register(UAV_REGISTER);

groupshared float2 gDepthSamples[ TERRAIN_COMPUTE_NUM_THREADS * TERRAIN_COMPUTE_NUM_THREADS ];
groupshared float4 gPlane;

[numthreads(TERRAIN_COMPUTE_NUM_THREADS, TERRAIN_COMPUTE_NUM_THREADS, 1)]
void cs_main(in uint3 GroupID : SV_GroupID, in uint3 GroupThreadID : SV_GroupThreadID, uint ThreadIndex : SV_GroupIndex)
{
}

#endif