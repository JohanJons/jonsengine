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
	dout.mPositionW = bary.x*tri[0].mPositionW + bary.y*tri[1].mPositionW + bary.z*tri[2].mPositionW;
	dout.mNormalW = bary.x*tri[0].mNormalW + bary.y*tri[1].mNormalW + bary.z*tri[2].mNormalW;
	dout.mTexcoordW = bary.x*tri[0].mTexcoordW + bary.y*tri[1].mTexcoordW + bary.z*tri[2].mTexcoordW;

	// Interpolating normal can unnormalize it, so normalize it.
	dout.mNormalW = normalize(dout.mNormalW);

	//
	// Displacement mapping.
	//

	// Choose the mipmap level based on distance to the eye; specifically, choose
	// the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	const float MipInterval = 20.0f;
	//float mipLevel = clamp((distance(dout.PosW, gEyePosW) - MipInterval) / MipInterval, 0.0f, 6.0f);

	// Sample height map (stored in alpha channel).
	//float h = gNormalMap.SampleLevel(samLinear, dout.Tex, mipLevel).a;

	// Offset vertex along normal.
	//dout.PosW += (gHeightScale*(h - 1.0))*dout.NormalW;
	dout.mPositionW += 1.0;

	// Project to homogeneous clip space.
	dout.mPosition = 1.0;// mul(float4(dout.PosW, 1.0f), gViewProj);

	return dout;
}

#endif