#ifndef DEPTH_READBACK_HLSL
#define DEPTH_READBACK_HLSL

#include "Constants.h"
#include "Common.hlsl"

cbuffer DepthReadbackConstants : register(CBUFFER_REGISTER_COMPUTE)
{
	float2 gUVCoords;
};

[numthreads(1, 1, 1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{

}

#endif