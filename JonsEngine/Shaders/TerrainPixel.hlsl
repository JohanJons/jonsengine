#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"

Texture2D gNormalMap : register(TEXTURE_REGISTER_NORMAL);

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
#ifndef TERRAIN_NORMAL_DEBUG
	float3 mNormal : SV_Target1;
#endif
};

PixelOut ps_main( VertexOut input)
{
	PixelOut ret;

#if TERRAIN_NORMAL_SIMPLE
	float3 worldNormal = gNormalMap.Sample( gLinearSampler, input.mTexcoord ).xyz;
#elif TERRAIN_NORMAL_BETTER
	float3 worldNormal = gNormalMap.Sample( gAnisotropicSampler, input.mTexcoord ).xyz;
#else
	float3 worldNormal = float3( 1.0f, 1.0f, 1.0f );
#endif

#if TERRAIN_NORMAL_DEBUG
	ret.mDiffuse = float4( worldNormal, 1.0f );
#else
	float3 viewNormal = mul( ( float3x3 )gFrameView, worldNormal );
	viewNormal = normalize( viewNormal );
	viewNormal += 1.0f;
	viewNormal *= 0.5f;

	ret.mNormal = viewNormal;
	ret.mDiffuse = float4( 0.5f, 0.5f, 1.0f, 1.0f);
#endif

	return ret;
}

#endif