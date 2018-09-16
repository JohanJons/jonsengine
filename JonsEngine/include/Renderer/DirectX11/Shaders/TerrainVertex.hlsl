#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float4 mPosition : POSITION;
	float3 mNormal : NORMAL;
	uint mInstanceID : SV_InstanceID;
};

StructuredBuffer<float4x4> gWorldTransforms : register (SBUFFER_REGISTER_EXTRA);
Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );

float2 WorldXZToTexcoord(float2 worldMinExtent, float2 worldMaxExtent, float2 worldPos)
{
	return abs((worldPos - worldMinExtent) / (worldMaxExtent - worldMinExtent));
}

VertexOut vs_main(VertexIn input)
{
	VertexOut ret;

	const uint transformIndex = input.mInstanceID;
	// silly that we have to transpose this...
	const float4x4 worldTransform = transpose( gWorldTransforms.Load( transformIndex ) );

	ret.mWorldPosition = mul( worldTransform, input.mPosition );
	ret.mWorldNormal = mul( (float3x3)worldTransform, input.mNormal );
	ret.mTessFactor = 2.0f;

	ret.mTexcoord = 1.0f / ( float2(gWorldMaxX, gWorldMaxZ) + ( -float2(gWorldMinX, gWorldMinZ) ) );
	ret.mTexcoord = ( -float2( gWorldMinX, gWorldMinZ ) + ret.mWorldPosition.xz ) * ret.mTexcoord;
	ret.mTexcoord = clamp( ret.mTexcoord, 0.0f, 1.0f );

	return ret;
}

#endif