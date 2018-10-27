#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
	float3 mNormal : SV_Target1;
};


PixelOut ps_main(DomainOut input)
{
	PixelOut ret;

	float3 dFdxPos = ddx( input.mViewPosition.xyz );
	float3 dFdyPos = ddy( input.mViewPosition.xyz );
	ret.mNormal = normalize( cross( dFdxPos, dFdyPos ) );
	ret.mNormal += 1.0f;
	ret.mNormal *= 0.5f;

	ret.mDiffuse = float4( 0.0f, 1.0f, 1.0f, 1.0f);

	return ret;
}

#endif