#ifndef TRANSFORM_VERTEX_HLSL
#define TRANSFORM_VERTEX_HLSL

#include "Constants.h"

cbuffer TransformConstants : register(CBUFFER_REGISTER_VERTEX)
{
    float4x4 gWVPMatrix;
};


float4 vs_main(float3 position : POSITION) : SV_Position
{
    return mul(gWVPMatrix, float4(position, 1.0f));
}

#endif