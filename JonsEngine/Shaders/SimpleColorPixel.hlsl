#ifndef SIMPLE_COLOR_PIXEL_HLSL
#define SIMPLE_COLOR_PIXEL_HLSL

#include "Constants.h"

cbuffer ColorConstants : register(CBUFFER_REGISTER_PIXEL)
{
	float3 gColor;
};

float4 ps_main() : SV_Target0
{
	return float4( gColor, 0.0 );
}

#endif