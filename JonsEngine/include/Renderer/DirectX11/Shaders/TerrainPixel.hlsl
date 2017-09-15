#ifndef TERRAIN_PIXEL_HLSL
#define TERRAIN_PIXEL_HLSL

#include "TerrainCommon.hlsl"

struct PixelOut
{
	float4 mDiffuse : SV_Target0;
	//float3 mNormal : SV_Target1;
};


PixelOut ps_main(VertexOut input)
{
	PixelOut ret;

	ret.mDiffuse = float4(0.0, 1.0, 0.0, 0.0);
	//ret.mNormal = normalize(input.mNormalW);
	//ret.mNormal += 1.0;
	//ret.mNormal *= 0.5;

	return ret;
}

#endif