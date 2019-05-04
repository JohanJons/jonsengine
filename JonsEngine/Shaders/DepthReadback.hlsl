#ifndef DEPTH_READBACK_HLSL
#define DEPTH_READBACK_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"


Texture2D gDepthTexture : register(TEXTURE_REGISTER_DEPTH);
RWBuffer<float> gOutputDepth : register(UAV_REGISTER);

cbuffer DepthReadbackConstants : register(CBUFFER_REGISTER_COMPUTE)
{
	uint2 gDepthTexcoords;
};

[numthreads(1, 1, 1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{
	gOutputDepth[0] = gDepthTexture[gDepthTexcoords].x;
}

#endif