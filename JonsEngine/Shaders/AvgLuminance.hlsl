#ifndef AVG_LUMINANCE_HLSL
#define AVG_LUMINANCE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "FullscreenTriangleTexcoord.hlsl"

cbuffer AvgLuminanceConstants : register(CBUFFER_REGISTER_PIXEL)
{
	float gTimeDeltaSeconds;
	float gAdaptationRate;
};

Texture2D gFrameTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gPreviousLumTexture : register(TEXTURE_REGISTER_EXTRA);

float4 ps_main(FullScreenTexcoordOutput input) : SV_Target0
{
	const float3 rgb = gFrameTexture.Sample(gLinearSampler, input.mTexcoord).rgb;
	const float currLum = CalcLuminance(rgb);

	const float lastLum = exp(gPreviousLumTexture.Sample(gPointSampler, input.mTexcoord).r);
	const float adaptedLum = lastLum + (currLum - lastLum) * (1 - exp(-gTimeDeltaSeconds * gAdaptationRate));

	return float4(log(adaptedLum), 1.0f, 1.0f, 1.0f);
}

#endif