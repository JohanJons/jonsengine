#ifndef GBUFFER_PIXEL_HLSL
#define GBUFFER_PIXEL_HLSL

#include "Common.hlsl"
#include "GBufferVertex.hlsl"

struct GBufferPSOut
{
    float4 mDiffuse : SV_Target0;
    float4 mNormal : SV_Target1;
};

Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
SamplerState gSampler : register(SAMPLER_REGISTER_ANISOTROPIC);


GBufferPSOut ps_main(GBufferVSOut input)
{
    GBufferPSOut output;

    if (gHasDiffuseTexture)
        output.mDiffuse = gDiffuseTexture.Sample(gSampler, input.mTexcoord);
    else
        output.mDiffuse = float4(1.0, 1.0, 1.0, 1.0);

    if (gHasNormalTexture)
    {
        float3 normalTangentSpace = normalize(gNormalTexture.Sample(gSampler, input.mTexcoord) * 2.0 - 1.0);
        float3x3 tangentToWorldSpace = CreateMatrixFromCols(normalize(input.mTangent), normalize(input.mBitangent), normalize(input.mNormal));

        output.mNormal = mul(tangentToWorldSpace, normalTangentSpace);
    }
    else
        output.mNormal = normalize(input.mNormal);

    return output;
}

#endif