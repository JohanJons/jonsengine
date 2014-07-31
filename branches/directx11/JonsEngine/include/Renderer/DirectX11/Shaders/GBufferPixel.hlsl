#ifndef GBUFFER_PIXEL_HLSL
#define GBUFFER_PIXEL_HLSL

#include "GBufferVertex.hlsl"

struct GBufferPSOut
{
    float4 mWorldPosition : SV_Target0;
    float4 mDiffuse : SV_Target1;
    float4 mNormal : SV_Target2;
};


GBufferPSOut ps_main(GBufferVSOut input)
{
    GBufferPSOut output;

    output.mWorldPosition = input.mWorldPosition;

    if (gHasDiffuseTexture)
        output.mDiffuse = gDiffuseTexture.Sample(gSampler, input.mTexcoord);
    else
        output.mDiffuse = float4(1.0, 1.0, 1.0, 1.0);

    if (gHasNormalTexture)
    {
        float4 normalTangentSpace = normalize(gNormalTexture.Sample(gSampler, input.mTexcoord) * 2.0 - 1.0);
        float4x4 tangentToWorldSpace = float4x4(normalize(input.mTangent), normalize(input.mBitangent), normalize(input.mNormal), float4(0.0, 0.0, 0.0, 1.0));

        output.mNormal = mul(tangentToWorldSpace, normalTangentSpace);
    }
    else
        output.mNormal = normalize(input.mNormal);

    return output;
}

#endif