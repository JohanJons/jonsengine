#ifndef NULL_VERTEX_HLSL
#define NULL_VERTEX_HLSL

cbuffer NullConstants : register(b0)
{
    float4x4 gWVPMatrix;
};


float4 vs_main(float3 position : POSITION) : SV_Position
{
    return mul(gWVPMatrix, float4(position, 1.0f));
}

#endif