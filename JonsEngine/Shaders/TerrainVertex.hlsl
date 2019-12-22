#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float3 mPosition : POSITION;
};

StructuredBuffer<float2> gLODMorphConstants : register ( SBUFFER_REGISTER_EXTRA );

float2 GetTextureCoordinates( float3 worldPos )
{
	float2 texcoord = ( worldPos.xz - gWorldMin ) / ( gWorldMax - gWorldMin );
	texcoord = clamp( texcoord, 0.0f, 1.0f );

	return texcoord;
}

float SampleHeightmap( float2 uv )
{
	int mipmap = 0;
	int offset = 0;

	return gHeightmap.SampleLevel( gLinearSampler, uv, mipmap, offset ).r;
}

VertexOut vs_main(VertexIn input)
{
	float4 worldPos = mul( gQuadWorldTransform, float4( input.mPosition, 1 ) );
	float2 preMorphTexcoord = GetTextureCoordinates( worldPos.xyz );
	worldPos.z = SampleHeightmap( preMorphTexcoord );

	// To be used
	float4 unmorphedPos = float4( worldPos.xyz, 1.0f );

	float cameraDistanceToVertex = distance( worldPos, gWorldEyePos );

	VertexOut ret;
	ret.mPosition = mul( gFrameViewProj, worldPos );;

	ret.mTexcoord = ( ret.mWorldPosition.xz - gWorldMin ) / ( gWorldMax - gWorldMin );
	ret.mTexcoord = clamp( ret.mTexcoord, 0.0f, 1.0f );

	//ret.mInstanceID = input.mInstanceID;

	return ret;
}

#endif