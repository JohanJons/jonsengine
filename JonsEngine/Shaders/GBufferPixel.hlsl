#ifndef GBUFFER_PIXEL_HLSL
#define GBUFFER_PIXEL_HLSL

#include "Common.hlsl"
#include "GBufferVertex.hlsl"

struct GBufferPSOut
{
    float4 mDiffuse : SV_Target0;
    float3 mNormal : SV_Target1;
};

Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);


GBufferPSOut ps_main(GBufferVSOut input)
{
    GBufferPSOut output;

    if (gHasDiffuseTexture)
        output.mDiffuse = gDiffuseTexture.Sample(gAnisotropicSampler, input.mTexcoord);
    else
        output.mDiffuse = float4(1.0, 1.0, 1.0, 1.0);

    if (gHasNormalTexture)
    {
        const float3 normalTangentSpace = normalize(gNormalTexture.Sample(gAnisotropicSampler, input.mTexcoord).xyz * 2.0 - 1.0);
        const float3x3 tangentToWorldSpace = CreateMatrixFromCols(normalize(input.mTangent), normalize(input.mBitangent), normalize(input.mNormal));

        output.mNormal = mul(tangentToWorldSpace, normalTangentSpace);
    }
    else
        output.mNormal = normalize(input.mNormal);

    output.mNormal += 1.0;
    output.mNormal *= 0.5;

    return output;
}

#endif