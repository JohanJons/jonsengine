#ifndef SDSM_INITIAL_COMPUTE_HLSL
#define SDSM_INITIAL_COMPUTE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"

// shared memory
groupshared float2 depthSamples[SDSM_NUM_THREADS];

Texture2D<float> gDepthTexture : register(TEXTURE_REGISTER_DEPTH);
RWTexture2D<unorm float2> gOutputMap : register(UAV_REGISTER);

cbuffer SDSMConstants : register(CBUFFER_REGISTER_COMPUTE)
{
    float gProjection33;
    float gProjection43;
    float gNearClip;
    float gFarClip;
};


[numthreads(SDSM_THREAD_GROUP_SIZE, SDSM_THREAD_GROUP_SIZE, 1)]
void cs_main(in uint3 GroupID : SV_GroupID, in uint3 GroupThreadID : SV_GroupThreadID, uint ThreadIndex : SV_GroupIndex)
{
    float minDepth = 1.0f;
    float maxDepth = 0.0f;

    uint2 textureSize;
    gDepthTexture.GetDimensions(textureSize.x, textureSize.y);

    uint2 samplePos = GroupID.xy * SDSM_THREAD_GROUP_SIZE + GroupThreadID.xy;
    samplePos = min(samplePos, textureSize - 1);

    float depthSample = gDepthTexture[samplePos];

    if (depthSample < 1.0f)
    {
        // Convert to linear Z
        depthSample = gProjection43 / (depthSample - gProjection33);
        depthSample = saturate((depthSample - gNearClip) / (gFarClip - gNearClip));
        minDepth = min(minDepth, depthSample);
        maxDepth = max(maxDepth, depthSample);
    }

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