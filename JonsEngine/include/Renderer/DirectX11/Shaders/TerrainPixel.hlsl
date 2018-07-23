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

	ret.mDiffuse = float4( input.mTexcoord, 0.0f, 1.0f);
	ret.mNormal = normalize(input.mNormal);
	ret.mNormal += 1.0f;
	ret.mNormal *= 0.5f;

	return ret;
}

#endif