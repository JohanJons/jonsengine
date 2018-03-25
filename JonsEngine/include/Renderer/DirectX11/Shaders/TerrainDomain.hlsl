#ifndef TERRAIN_DOMAIN_HLSL
#define TERRAIN_DOMAIN_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

[domain("quad")]
DomainOut domain_main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
	DomainOut ret;

	float2 topMidpoint = lerp( quad[ 0 ].mWorldPosition.xz, quad[ 1 ].mWorldPosition.xz, uv.x );
	float2 bottomMidpoint = lerp( quad[ 3 ].mWorldPosition.xz, quad[ 2 ].mWorldPosition.xz, uv.x );
	float2 midPoint = lerp( topMidpoint, bottomMidpoint, uv.y );

	ret.mPosition = float4( midPoint.x, quad[ 0 ].mWorldPosition.y, midPoint.y, 1 );
	ret.mPosition = mul( gFrameViewProj, ret.mPosition );
	ret.mNormal = mul( (float3x3)gFrameView, float3( 0.0f, 1.0f, 0.0f ) );

	return ret;
}

#endif