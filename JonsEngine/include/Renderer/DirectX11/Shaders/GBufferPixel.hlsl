#ifndef GBUFFER_PIXEL_HLSL
#define GBUFFER_PIXEL_HLSL

#include "GBufferVertex.hlsl"

struct GBufferPSOut
{
    float4 mWorldPosition : SV_Target0;
    float4 mDiffuse : SV_Target1;
    float4 mNormal : SV_Target2;
};

Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gNormalTexture : register(TEXTURE_REGISTER_NORMAL);
SamplerState gSampler : register(SAMPLER_REGISTER_ANISOTROPIC);


// needed due to float4x4 constructor is row-major
float4x4 CreateMatrixFromCols(float4 c0, float4 c1, float4 c2, float4 c3)
{
    return float4x4(c0.x, c1.x, c2.x, c3.x,
                    c0.y, c1.y, c2.y, c3.y,
                    c0.z, c1.z, c2.z, c3.z,
                    c0.w, c1.w, c2.w, c3.w);
}

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
        float4x4 tangentToWorldSpace = CreateMatrixFromCols(normalize(input.mTangent), normalize(input.mBitangent), normalize(input.mNormal), float4(0.0, 0.0, 0.0, 1.0));

        output.mNormal = mul(tangentToWorldSpace, normalTangentSpace);
    }
    else
        output.mNormal = normalize(input.mNormal);

    return output;
}

#endif