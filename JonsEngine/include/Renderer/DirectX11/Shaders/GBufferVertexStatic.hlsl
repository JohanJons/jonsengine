#ifndef GBUFFER_VERTEX_STATIC_HLSL
#define GBUFFER_VERTEX_STATIC_HLSL

#define GBUFFER_IS_ANIMATED_MESHES 0

struct GBufferVSIn
{
	float4 mPosition : POSITION;
	float3 mNormal : NORMAL;
	float2 mTexcoord : TEXCOORD;
	float3 mTangent : TANGENT;
	float3 mBitangent : BITANGENT;
};

#include "GbufferVertex.hlsl"

#endif