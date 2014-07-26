#ifndef GBUFFER_VERTEX_HLSL
#define GBUFFER_VERTEX_HLSL

#define GBUFFER_CONSTANTS
#include "Constants.hlsl"

GBufferVSOut main(GBufferVSIn input)
{
    GBufferVSOut output;

    output.mPosition = mul(gWVPMatrix, float4(input.mPosition, 0));
    output.mWorldPosition = (float3)mul(gWorldMatrix, float4(input.mPosition, 0));
    output.mTexcoord = gTextureTilingFactor * input.mTexcoord;

    return output;
}

#endif