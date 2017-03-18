#ifndef TERRAIN_DOMAIN_HLSL
#define TERRAIN_DOMAIN_HLSL

#include "TerrainCommon.hlsl"

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("tri")]
DomainOut domain_main(PatchTess patchTess, float3 bary : SV_DomainLocation, const OutputPatch<HullOut, 3> tri)
{
	DomainOut dout;

	// Interpolate patch attributes to generated vertices.
	dout.PosW = bary.x*tri[0].PosW + bary.y*tri[1].PosW + bary.z*tri[2].PosW;
	dout.NormalW = bary.x*tri[0].NormalW + bary.y*tri[1].NormalW + bary.z*tri[2].NormalW;
	dout.TangentW = bary.x*tri[0].TangentW + bary.y*tri[1].TangentW + bary.z*tri[2].TangentW;
	dout.Tex = bary.x*tri[0].Tex + bary.y*tri[1].Tex + bary.z*tri[2].Tex;

	// Interpolating normal can unnormalize it, so normalize it.
	dout.NormalW = normalize(dout.NormalW);

	//
	// Displacement mapping.
	//

	// Choose the mipmap level based on distance to the eye; specifically, choose
	// the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	const float MipInterval = 20.0f;
	float mipLevel = clamp((distance(dout.PosW, gEyePosW) - MipInterval) / MipInterval, 0.0f, 6.0f);

	// Sample height map (stored in alpha channel).
	float h = gNormalMap.SampleLevel(samLinear, dout.Tex, mipLevel).a;

	// Offset vertex along normal.
	dout.PosW += (gHeightScale*(h - 1.0))*dout.NormalW;

	// Project to homogeneous clip space.
	dout.PosH = mul(float4(dout.PosW, 1.0f), gViewProj);

	return dout;
}

#endif