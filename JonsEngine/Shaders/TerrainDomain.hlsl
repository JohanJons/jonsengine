#ifndef TERRAIN_DOMAIN_HLSL
#define TERRAIN_DOMAIN_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
Texture2D gPerlinNoiseTexture : register( TEXTURE_REGISTER_PERLIN );

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

/*float3 CalculateNormal( float2 uv )
{
	const float y1 = SampleHeightmap( uv, int2( 0, -1 ) );
	const float y2 = SampleHeightmap( uv, int2( -1, 0 ) );
	const float y3 = SampleHeightmap( uv, int2( 1, 0 ) );
	const float y4 = SampleHeightmap( uv, int2( 0, 1 ) );

	float3 normal = float3( y1 - y4, 0.1, y2 - y3 );

	return normalize( normal );
}*/

float3 SobelFilter2( float2 uv )
{
	const int2 offset = 0;
	const int mipmap = 0;

	float h00 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( -1, -1 ) ).r;
	float h10 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( 0, -1 ) ).r;
	float h20 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( 1, -1 ) ).r;

	float h01 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( -1, 0 ) ).r;
	float h21 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( 1, 0 ) ).r;

	float h02 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( -1, 1 ) ).r;
	float h12 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( 0, 1 ) ).r;
	float h22 = gHeightmap.SampleLevel( gPointSampler, uv, mipmap, int2( 1, 1 ) ).r;

	float Gx = h00 - h20 + 2.0f * h01 - 2.0f * h21 + h02 - h22;
	float Gy = h00 + 2.0f * h10 + h20 - h02 - 2.0f * h12 - h22;
	// generate missing Z
	float Gz = 0.01f * sqrt( max( 0.0f, 1.0f - Gx * Gx - Gy * Gy ) );

	return normalize( float3( 2.0f * Gx, Gz, 2.0f * Gy ) );

	//float Gx = -h00 - 2.0f * h10 - h20 + h02 + 2.0f * h12 + h22;
	//float Gy = -h02 - 2.0f * h01 - h00 + h22 + 2.0f * h21 + h20;
	//float h = -im1p1 - 2.0 * i0p1 - ip1p1 + im1m1 + 2.0 * i0m1 + ip1m1;
	//float v = -im1m1 - 2.0 * im10 - im1p1 + ip1m1 + 2.0 * ip10 + ip1p1;
	//float mag = length(float2(Gx, Gy));
	//return normalize( float3( mag, mag, mag ) );
}

[domain("quad")]
DomainOut ds_main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
	DomainOut ret;

	float2 topMidpointWorld = lerp( quad[ 0 ].mWorldPosition.xz, quad[ 1 ].mWorldPosition.xz, uv.x );
	float2 bottomMidpointWorld = lerp( quad[ 3 ].mWorldPosition.xz, quad[ 2 ].mWorldPosition.xz, uv.x );
	float2 midPointWorld = lerp( topMidpointWorld, bottomMidpointWorld, uv.y );

	float2 topMidpointTexcoord = lerp( quad[ 0 ].mTexcoord, quad[ 1 ].mTexcoord, uv.x );
	float2 bottomMidpointTexcoord = lerp( quad[ 3 ].mTexcoord, quad[ 2 ].mTexcoord, uv.x );
	float2 midPointTexcoord = lerp( topMidpointTexcoord, bottomMidpointTexcoord, uv.y );

	/*const int2 offset = 0;
	const int mipmap = 0;
	ret.mNormal = gNormalMap.SampleLevel( gLinearSampler, midPointTexcoord, mipmap, offset ).r;
	ret.mNormal *= 2.0;
	ret.mNormal -= 1.0;
	ret.mNormal = normalize( ret.mNormal );
	ret.mNormal.y = -ret.mNormal.y;*/

	//ret.mNormal.y = -ret.mNormal.y;
	//ret.mNormal = GetNormal( midPointWorld );
	//ret.mNormal = CalculateNormal( midPointTexcoord );
	ret.mNormal = SobelFilter2( midPointTexcoord );

	// TODO: texcoord
	float variation = gPerlinNoiseTexture.SampleLevel( gLinearWrapSampler, midPointTexcoord, 0 ).r;
	variation = ( variation * 2.0 ) - 1.0;
	variation *= gVariationScale;
	float4 variationVec = float4( variation * ret.mNormal, 0 );

	ret.mNormal = mul( ( float3x3 )gFrameView, ( float3 )ret.mNormal );
	ret.mNormal = normalize( ret.mNormal );

	float y = quad[ 0 ].mWorldPosition.y + ( SampleHeightmap( midPointTexcoord ) * gHeightModifier );

	ret.mPosition = float4( midPointWorld.x, y, midPointWorld.y, 1.0 );
	//ret.mPosition += variationVec;
	ret.mPosition = mul( gFrameViewProj, ret.mPosition );

	ret.mTexcoord = midPointTexcoord;

	// debug; these could be hidden if necessary
#ifdef TERRAIN_DEBUG_COPLANARITY
	ret.mCoplanarity = GetCoplanarity( midPointWorld );
#endif
	ret.mTesellation = patchTess.mInsideTess[ 0 ];

	return ret;
}

#endif