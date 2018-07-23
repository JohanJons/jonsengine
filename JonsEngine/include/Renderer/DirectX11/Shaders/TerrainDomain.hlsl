#ifndef TERRAIN_DOMAIN_HLSL
#define TERRAIN_DOMAIN_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );

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

	float2 topMidpointTexcoord = lerp( quad[ 0 ].mTexcoord, quad[ 1 ].mTexcoord, uv.x );
	float2 bottomMidpointTexcoord = lerp( quad[ 3 ].mTexcoord, quad[ 2 ].mTexcoord, uv.x );
	float2 midPointTexcoord = lerp( topMidpointTexcoord, bottomMidpointTexcoord, uv.y );

	float y = quad[ 0 ].mWorldPosition.y;
	y += ( gHeightmap[ midPointTexcoord ].x * gHeightModifier );

	//float2 texcoord =  1.0f / ( gWorldMax + ( -gWorldMin ) );
	//texcoord = ( -gWorldMin * midPoint ) * texcoord;
	//texcoord = clamp( texcoord, 0.0f, 1.0f );

	//float2 texcoord = 1.0f / ( float2( gWorldMaxX, gWorldMaxZ ) + ( -float2( gWorldMinX, gWorldMinZ ) ) );
	//texcoord = ( -float2( gWorldMinX, gWorldMinZ ) + midPoint ) * texcoord;
	//texcoord = clamp( texcoord, 0.0f, 1.0f );

	ret.mPosition = float4( midPointWorld.x, y, midPointWorld.y, 1 );
	ret.mPosition = mul( gFrameViewProj, ret.mPosition );
	ret.mNormal = mul( (float3x3)gFrameView, float3( 0.0f, 1.0f, 0.0f ) );
	ret.mTexcoord = midPointTexcoord;

	return ret;
}

#endif