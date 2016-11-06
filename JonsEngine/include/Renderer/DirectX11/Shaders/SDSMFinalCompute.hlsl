#ifndef SDSM_FINAL_COMPUTE_HLSL
#define SDSM_FINAL_COMPUTE_HLSL

#include "Constants.h"
#include "Common.hlsl"

// shared memory
groupshared float2 depthSamples[SDSM_NUM_THREADS];

Texture2D<float2> gReductionTexture : register(TEXTURE_REGISTER_EXTRA);
RWTexture2D<float2> gOutputMap : register(UAV_REGISTER);


[numthreads(SDSM_THREAD_GROUP_SIZE, SDSM_THREAD_GROUP_SIZE, 1)]
void cs_main(in uint3 GroupID : SV_GroupID, in uint3 GroupThreadID : SV_GroupThreadID, in uint ThreadIndex : SV_GroupIndex)
{
    uint2 textureSize;
    gReductionTexture.GetDimensions(textureSize.x, textureSize.y);

    uint2 samplePos = GroupID.xy * SDSM_THREAD_GROUP_SIZE + GroupThreadID.xy;
    samplePos = min(samplePos, textureSize - 1);

    float minDepth = gReductionTexture[samplePos].x;
    float maxDepth = gReductionTexture[samplePos].y;

    if (minDepth == 0.0f)
        minDepth = 1.0f;

    // Store in shared memory
    depthSamples[ThreadIndex] = float2(minDepth, maxDepth);
    GroupMemoryBarrierWithGroupSync();

    // Reduce
    [unroll]
    for (uint s = SDSM_NUM_THREADS / 2; s > 0; s >>= 1)
    {
        if (ThreadIndex < s)
        {
            depthSamples[ThreadIndex].x = min(depthSamples[ThreadIndex].x, depthSamples[ThreadIndex + s].x);
            depthSamples[ThreadIndex].y = max(depthSamples[ThreadIndex].y, depthSamples[ThreadIndex + s].y);
        }

        GroupMemoryBarrierWithGroupSync();
    }

    if (ThreadIndex == 0)
    {
        minDepth = depthSamples[0].x;
        maxDepth = depthSamples[0].y;
        gOutputMap[GroupID.xy] = float2(minDepth, maxDepth);
    }
}


#endif