#ifndef TERRAIN_DOMAIN_HLSL
#define TERRAIN_DOMAIN_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

Texture2D gNormalmap : register( TEXTURE_REGISTER_NORMAL );
Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
SamplerState gPointSampler : register( SAMPLER_REGISTER_POINT );

float2 GetMidpoint( const OutputPatch<HullOut, 4> quad, float2 uv )
{
	float2 topMidpoint = lerp( quad[ 0 ].mWorldPosition.xz, quad[ 1 ].mWorldPosition.xz, uv.x );
	float2 bottomMidpoint = lerp( quad[ 3 ].mWorldPosition.xz, quad[ 2 ].mWorldPosition.xz, uv.x );
	float2 midPoint = lerp( topMidpoint, bottomMidpoint, uv.y );

	return midPoint;
}

[domain("quad")]
DomainOut domain_main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
	DomainOut ret;

	float2 topMidpointWorld = lerp( quad[ 0 ].mWorldPosition.xz, quad[ 1 ].mWorldPosition.xz, uv.x );
	float2 bottomMidpointWorld = lerp( quad[ 3 ].mWorldPosition.xz, quad[ 2 ].mWorldPosition.xz, uv.x );
	float2 midPointWorld = lerp( topMidpointWorld, bottomMidpointWorld, uv.y );

	float3 topMidpointN = lerp( quad[ 0 ].mWorldNormal.xyz, quad[ 1 ].mWorldNormal.xyz, uv.x );
	float3 bottomMidpointN = lerp( quad[ 3 ].mWorldNormal.xyz, quad[ 2 ].mWorldNormal.xyz, uv.x );
	float3 midPointN = lerp( topMidpointN, bottomMidpointN, uv.y );

	float2 topMidpointTexcoord = lerp( quad[ 0 ].mTexcoord, quad[ 1 ].mTexcoord, uv.x );
	float2 bottomMidpointTexcoord = lerp( quad[ 3 ].mTexcoord, quad[ 2 ].mTexcoord, uv.x );
	float2 midPointTexcoord = lerp( topMidpointTexcoord, bottomMidpointTexcoord, uv.y );

	float y = quad[ 0 ].mWorldPosition.y;
	y += ( gHeightmap.SampleLevel( gPointSampler, midPointTexcoord, 0.0f ).r * gHeightModifier );

	ret.mPosition = float4( midPointWorld.x, y, midPointWorld.y, 1 );
	ret.mViewPosition = mul( gFrameView, ret.mPosition );
	ret.mPosition = mul( gFrameViewProj, ret.mPosition );

	ret.mTexcoord = midPointTexcoord;

	return ret;
}

#endif