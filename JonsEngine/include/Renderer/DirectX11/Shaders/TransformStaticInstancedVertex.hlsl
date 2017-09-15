#ifndef TRANSFORM_STATIC_INSTANCED_VERTEX_HLSL
#define TRANSFORM_STATIC_INSTANCED_VERTEX_HLSL

#include "Constants.h"
#include "Common.hlsl"

cbuffer TransformConstants : register(CBUFFER_REGISTER_VERTEX)
{
    float4x4 gViewProjectionMatrix;
};

StructuredBuffer<float4x4> gWorldTransforms : register (SBUFFER_REGISTER_EXTRA);


float4 vs_main(float4 position : POSITION, uint instanceID : SV_InstanceID) : SV_Position
{
	float4x4 worldMatrix = gWorldTransforms.Load(instanceID);
	float4x4 wvpMatrix = mul(gViewProjectionMatrix, worldMatrix);

	return mul(wvpMatrix, position);
}

#endif