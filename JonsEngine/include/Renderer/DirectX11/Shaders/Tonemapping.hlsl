#ifndef TONEMAPPING_VERTEX_HLSL
#define TONEMAPPING_VERTEX_HLSL

#include "Constants.h"
#include "Common.hlsl"
#include "FullscreenTriangleTexcoord.hlsl"

Texture2D gFrameTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gLuminanceTexture : register(TEXTURE_REGISTER_EXTRA);
SamplerState gLinearSampler : register(SAMPLER_REGISTER_LINEAR);

cbuffer TonemappingConstants : register(CBUFFER_REGISTER_PIXEL)
{
	unsigned int gMipMapLevel;
};

static const float A = 0.15f;
static const float B = 0.50f;
static const float C = 0.10f;
static const float D = 0.20f;
static const float E = 0.02f;
static const float F = 0.30f;
static const float W = 11.2f;
#define TonemappingU2(color) (((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F)
static const float gWhiteTonemapped = TonemappingU2(W);


float4 ps_main(FullScreenTexcoordOutput input) : SV_Target0
{
	float3 color = gFrameTexture.Sample(gLinearSampler, input.mTexcoord).rgb;
	float avgLuminance = exp(gLuminanceTexture.SampleLevel(gLinearSampler, input.mTexcoord, gMipMapLevel).r);

	float linearExposure = 1.03f - (2.0f / (2 + log10(avgLuminance + 1)));
	linearExposure = (linearExposure / avgLuminance);
	color *= linearExposure;

	color = TonemappingU2(color) / gWhiteTonemapped;

	return float4(color, 1.0f);
}

#endif