#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float3 mPosition : POSITION;
	uint mInstanceID : SV_InstanceID;
};

StructuredBuffer<float4x4> gWorldTransforms : register (SBUFFER_REGISTER_EXTRA);

VertexOut vs_main(VertexIn input)
{
	VertexOut ret;

	const uint transformIndex = gTransformOffset + input.mInstanceID;
	// silly that we have to transpose this...
	const float4x4 worldTransform = transpose( gWorldTransforms.Load( transformIndex ) );

	ret.mWorldPosition = mul( worldTransform, float4( input.mPosition, 1 ) ).xyz;

	ret.mTexcoord = 1.0f / ( float2(gWorldMaxX, gWorldMaxZ) + ( -float2(gWorldMinX, gWorldMinZ) ) );
	ret.mTexcoord = ( -float2( gWorldMinX, gWorldMinZ ) + ret.mWorldPosition.xz ) * ret.mTexcoord;
	ret.mTexcoord = clamp( ret.mTexcoord, 0.0f, 1.0f );

	return ret;
}

#endif