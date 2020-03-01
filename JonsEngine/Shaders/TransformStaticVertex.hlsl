#ifndef TRANSFORM_STATIC_VERTEX_HLSL
#define TRANSFORM_STATIC_VERTEX_HLSL

#include "Constants.hlsl"

cbuffer TransformConstants : register(CBUFFER_REGISTER_VERTEX)
{
    float4x4 gWVPMatrix;
	uint gBoneIndexOffset;
};


float4 vs_main(float4 position : POSITION) : SV_Position
{
	return mul(gWVPMatrix, position);
}

#endif