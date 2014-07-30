#ifndef GBUFFER_PIXEL_HLSL
#define GBUFFER_PIXEL_HLSL

#include "GBufferVertex.hlsl"

struct GBufferPSOut
{
    float4 mWorldPosition : SV_Target0;
    float4 mDiffuse : SV_Target1;
};

Texture2D gDiffuseTexture : register(t1);
SamplerState gSampler : register(s0);


GBufferPSOut ps_main(GBufferVSOut input)
{
    GBufferPSOut output;

    output.mWorldPosition = input.mWorldPosition;
    output.mDiffuse = gDiffuseTexture.Sample(gSampler, input.mTexcoord);

    return output;
}

#endif