#ifndef AMBIENT_PIXEL_HLSL
#define AMBIENT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"
#include "Constants.hlsl"

cbuffer AmbientConstants : register(CBUFFER_REGISTER_PIXEL)
{
    float4 gAmbientLight;
};

Texture2D gDiffuseTexture : register(TEXTURE_REGISTER_DIFFUSE);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    return gDiffuseTexture[uint2(position.xy)] * gAmbientLight;
}

#endif