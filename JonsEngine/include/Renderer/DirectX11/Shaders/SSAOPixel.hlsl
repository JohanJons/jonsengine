#ifndef SSAO_PIXEL_HLSL
#define SSAO_PIXEL_HLSL

#include "Constants.h"
#include "Common.hlsl"

static const float gKernelSize = 16.0;
static const float gRadius = 1.5;
static const float gSSAOPower = 2.0;
static const float3 gSSAOKernel[gKernelSize] =
{
    float3(-0.0780436, 0.0558389, 0.0281286),
    float3(0.034318, -0.0635879, 0.0741237),
    float3(0.00230821, 0.0807279, 0.0805477),
    float3(0.0124638, 0.117585, 0.0578601),
    float3(0.093943, -0.0944602, 0.0816459),
    float3(0.139348, -0.109816, 0.0618508),
    float3(-0.181872, -0.129649, 0.0380075),
    float3(0.240066, -0.0494057, 0.118559),
    float3(0.115965, -0.0374714, 0.301286),
    float3(-0.294819, -0.100726, 0.225789),
    float3(-0.149652, 0.37459, 0.202967),
    float3(0.261695, -0.292813, 0.349015),
    float3(-0.37944, -0.425145, 0.206921),
    float3(0.628994, -0.189387, 0.224343),
    float3(-0.331257, -0.646864, 0.307335),
    float3(-0.467004, 0.439687, 0.618459),
};


cbuffer SSAOCBuffer : register(CBUFFER_REGISTER_PIXEL)
{
    float4x4 gViewProjMatrix;
    float4x4 gProjMatrix;
    float4x4 gViewMatrix;
    float2 gScreenSize;
};

Texture2D gPositionTexture : register(TEXTURE_REGISTER_POSITION);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
Texture2D gNoiseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gDepthTexture : register(TEXTURE_REGISTER_DEPTH);
SamplerState gPointSampler : register(SAMPLER_REGISTER_POINT);

float linearizeDepth(in float depth, in float4x4 projMatrix) {
    return projMatrix[3][2] / (depth - projMatrix[2][2]);
}

float4 ssao(in float3x3 kernelBasis, in float3 originPos)
{
    float occlusion = 0.0;
    for (int i = 0; i < gKernelSize; i++)
    {
        float3 samplePos = mul(kernelBasis, gSSAOKernel[i]);
        samplePos *= gRadius * originPos;

        float4 offset = mul(gViewProjMatrix, float4(samplePos, 1.0));
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        float depth = gDepthTexture.Sample(gPointSampler, offset.xy).r;
        depth = linearizeDepth(depth, gProjMatrix);

        //float rangeCheck = abs(originPos.z - depth) < gRadius ? 1.0 : 0.0;
        occlusion += (depth <= samplePos.z ? 1.0 : 0.0);// *rangeCheck;

    }
    occlusion = 1.0 - (occlusion / float(gKernelSize));

    return pow(occlusion, gSSAOPower);
}

float4 ps_main(float4 position : SV_Position) : SV_Target0
{
   /* float2 coords = float2(gScreenSize.x, gScreenSize.y) / float2(4.0, 4.0);
    coords *= float2(position.x / gScreenSize.x, position.y / gScreenSize.y);

    float3 originPos = gPositionTexture[uint2(position.xy)].xyz;
    float3 normal = gNormalTexture[uint2(position.xy)].xyz;

    originPos = mul(gViewMatrix, float4(originPos, 1.0)).xyz;
    normal = mul(gViewMatrix, float4(normal, 0.0)).xyz;

    float3 rvec = gNoiseTexture.Sample(gPointSampler, coords).xyz; //texture(uNoiseTex, noiseTexCoords).rgb * 2.0 - 1.0;
    float3 tangent = normalize(rvec - normal * dot(rvec, normal));
    float3 bitangent = cross(tangent, normal);
    float3x3 kernelBasis = CreateMatrixFromCols(tangent, bitangent, normal);

    return float4(rvec, 1.0);*/
   // return ssao(kernelBasis, originPos);// gFinalTexture.Sample(gPointSampler, coords);//float4(1.0, 0.0, 1.0, 1.0);
    float3 originPos = gPositionTexture[uint2(position.xy)].xyz;
    float3 normal = gNormalTexture[uint2(position.xy)].xyz;

    //originPos = mul(gViewMatrix, float4(originPos, 1.0)).xyz;
    //normal = mul(gViewMatrix, float4(normal, 0.0)).xyz;

    float ambientOcclusion = 0.0;
    const float2 filterRadius = float2(0.0052, 0.0093);
    for (int i = 0; i < gKernelSize; i++)
    {
        float2 sampleTexCoords = position.xy / gScreenSize;
        sampleTexCoords += gSSAOKernel[i].xy * filterRadius;

        float3 samplePos = gPositionTexture.Sample(gPointSampler, sampleTexCoords).xyz;
        //samplePos = mul(gViewMatrix, float4(samplePos, 1.0)).xyz;
        float3 sampleDir = normalize(samplePos - originPos);

        float NdotS = max(dot(normal, sampleDir), 0);
        float VPdistSP = distance(originPos, samplePos);

        float interp = 1.0 - smoothstep(5.0, 10.0, VPdistSP);

        ambientOcclusion += (interp * NdotS);
    }

    return 1.0 - (ambientOcclusion / gKernelSize);
    //return 1.0;
}

#endif