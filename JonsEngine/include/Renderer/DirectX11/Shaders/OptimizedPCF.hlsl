#ifndef OPTIMIZED_PCF_HLSL
#define OPTIMIZED_PCF_HLSL

//
// Thanks to the authors of "The Witness" for large parts of the shadercode
// NOTE: define PCF_KERNEL_SIZE prior to including this for different PCF kernels
//
#ifndef PCF_KERNEL_SIZE
    #define PCF_KERNEL_SIZE 2
#endif

//float SampleOptimizedPCF(in float3 shadowPos, in SamplerComparisonState shadowCmpSampler, in float shadowmapSize, in Texture2D shadowmap)
//{
//}

float SampleOptimizedPCF(in float3 shadowPos, in float shadowmapSize, in SamplerComparisonState shadowCmpSampler, in Texture2DArray shadowmapArr, in float cascadeIndex)
{
    float2 uv = shadowPos.xy * shadowmapSize; // 1 unit - 1 texel

        float2 shadowMapSizeInv = 1.0 / shadowmapSize;

        float2 base_uv;
    base_uv.x = floor(uv.x + 0.5);
    base_uv.y = floor(uv.y + 0.5);

    float s = (uv.x + 0.5 - base_uv.x);
    float t = (uv.y + 0.5 - base_uv.y);

    base_uv -= float2(0.5, 0.5);
    base_uv *= shadowMapSizeInv;

    float sum = 0;

#if PCF_KERNEL_SIZE == 2
    return shadowmapArr.SampleCmpLevelZero(shadowCmpSampler, float3(shadowPos.xy, cascadeIndex), lightDepth);
#elif PCF_KERNEL_SIZE == 3

    float uw0 = (3 - 2 * s);
    float uw1 = (1 + 2 * s);

    float u0 = (2 - s) / uw0 - 1;
    float u1 = s / uw1 + 1;

    float vw0 = (3 - 2 * t);
    float vw1 = (1 + 2 * t);

    float v0 = (2 - t) / vw0 - 1;
    float v1 = t / vw1 + 1;

    sum += uw0 * vw0 * SampleShadowMap(base_uv, u0, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw0 * SampleShadowMap(base_uv, u1, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw0 * vw1 * SampleShadowMap(base_uv, u0, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw1 * SampleShadowMap(base_uv, u1, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    return sum * 1.0f / 16;

#elif PCF_KERNEL_SIZE == 5

    float uw0 = (4 - 3 * s);
    float uw1 = 7;
    float uw2 = (1 + 3 * s);

    float u0 = (3 - 2 * s) / uw0 - 2;
    float u1 = (3 + s) / uw1;
    float u2 = s / uw2 + 2;

    float vw0 = (4 - 3 * t);
    float vw1 = 7;
    float vw2 = (1 + 3 * t);

    float v0 = (3 - 2 * t) / vw0 - 2;
    float v1 = (3 + t) / vw1;
    float v2 = t / vw2 + 2;

    sum += uw0 * vw0 * SampleShadowMap(base_uv, u0, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw0 * SampleShadowMap(base_uv, u1, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw0 * SampleShadowMap(base_uv, u2, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    sum += uw0 * vw1 * SampleShadowMap(base_uv, u0, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw1 * SampleShadowMap(base_uv, u1, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw1 * SampleShadowMap(base_uv, u2, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    sum += uw0 * vw2 * SampleShadowMap(base_uv, u0, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw2 * SampleShadowMap(base_uv, u1, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw2 * SampleShadowMap(base_uv, u2, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    return sum * 1.0f / 144;

#else // PCF_KERNEL_SIZE == 7

    float uw0 = (5 * s - 6);
    float uw1 = (11 * s - 28);
    float uw2 = -(11 * s + 17);
    float uw3 = -(5 * s + 1);

    float u0 = (4 * s - 5) / uw0 - 3;
    float u1 = (4 * s - 16) / uw1 - 1;
    float u2 = -(7 * s + 5) / uw2 + 1;
    float u3 = -s / uw3 + 3;

    float vw0 = (5 * t - 6);
    float vw1 = (11 * t - 28);
    float vw2 = -(11 * t + 17);
    float vw3 = -(5 * t + 1);

    float v0 = (4 * t - 5) / vw0 - 3;
    float v1 = (4 * t - 16) / vw1 - 1;
    float v2 = -(7 * t + 5) / vw2 + 1;
    float v3 = -t / vw3 + 3;

    sum += uw0 * vw0 * SampleShadowMap(base_uv, u0, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw0 * SampleShadowMap(base_uv, u1, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw0 * SampleShadowMap(base_uv, u2, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw3 * vw0 * SampleShadowMap(base_uv, u3, v0, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    sum += uw0 * vw1 * SampleShadowMap(base_uv, u0, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw1 * SampleShadowMap(base_uv, u1, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw1 * SampleShadowMap(base_uv, u2, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw3 * vw1 * SampleShadowMap(base_uv, u3, v1, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    sum += uw0 * vw2 * SampleShadowMap(base_uv, u0, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw2 * SampleShadowMap(base_uv, u1, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw2 * SampleShadowMap(base_uv, u2, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw3 * vw2 * SampleShadowMap(base_uv, u3, v2, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    sum += uw0 * vw3 * SampleShadowMap(base_uv, u0, v3, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw1 * vw3 * SampleShadowMap(base_uv, u1, v3, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw2 * vw3 * SampleShadowMap(base_uv, u2, v3, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);
    sum += uw3 * vw3 * SampleShadowMap(base_uv, u3, v3, shadowMapSizeInv, cascadeIdx, lightDepth, receiverPlaneDepthBias);

    return sum * 1.0f / 2704;
#endif
}

#endif