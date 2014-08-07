#ifndef AMBIENT_PIXEL_HLSL
#define AMBIENT_PIXEL_HLSL

#include "FullscreenTriangleVertex.hlsl"

cbuffer AmbientConstants : register(b0)
{
    float4 gAmbientLight;
};

Texture2D gDiffuseTexture : register(t1);


float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    return gDiffuseTexture[uint2(position.xy)] * gAmbientLight;
}

#endif