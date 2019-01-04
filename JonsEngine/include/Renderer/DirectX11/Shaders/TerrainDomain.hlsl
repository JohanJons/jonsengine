#ifndef TERRAIN_DOMAIN_HLSL
#define TERRAIN_DOMAIN_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

Texture2D gNormalmap : register( TEXTURE_REGISTER_NORMAL );
Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
Texture2D gPerlinNoiseTexture : register( TEXTURE_REGISTER_PERLIN );
SamplerState gPointSampler : register( SAMPLER_REGISTER_POINT );
SamplerState gLinearWrapSampler : register( SAMPLER_REGISTER_LINEAR_WRAP );

float SampleHeightmap( float2 uv, int2 offset )
{
	const int mipmap = 0;
	return gHeightmap.SampleLevel( gPointSampler, uv, mipmap, offset ).r;
}

float SampleHeightmap( float2 uv )
{
	const int2 offset = 0;
	return SampleHeightmap( uv, offset );
}

float3 CalculateNormal( float2 uv )
{
	const float y1 = SampleHeightmap( uv, int2( 0, -1 ) );
	const float y2 = SampleHeightmap( uv, int2( -1, 0 ) );
	const float y3 = SampleHeightmap( uv, int2( 1, 0 ) );
	const float y4 = SampleHeightmap( uv, int2( 0, 1 ) );

	float3 normal = float3( y1 - y4, 0.1, y2 - y3 );

	return normalize( normal );
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

	ret.mNormal = CalculateNormal( midPointTexcoord );

	float displacement = gPerlinNoiseTexture.SampleLevel( gLinearWrapSampler, midPointTexcoord, 0 ).r;
	float4 displaceVec = float4( displacement * ret.mNormal, 0 );

	ret.mNormal = mul( ( float3x3 )gFrameView, ( float3 )ret.mNormal );
	ret.mNormal = normalize( ret.mNormal );

	float y = quad[ 0 ].mWorldPosition.y + ( SampleHeightmap( midPointTexcoord ) * gHeightModifier );
	//float displacement = gPerlinNoiseTexture.SampleLevel( gLinearWrapSampler, midPointTexcoord, 0 ).r;

	ret.mPosition = float4( midPointWorld.x, y, midPointWorld.y, 1.0 );
	ret.mPosition += displaceVec;
	ret.mPosition = mul( gFrameViewProj, ret.mPosition );

	ret.mTexcoord = midPointTexcoord;

	return ret;
}

#endif