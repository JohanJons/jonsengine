#ifndef TONEMAPPING_VERTEX_HLSL
#define TONEMAPPING_VERTEX_HLSL

#include "Constants.h"
#include "Common.hlsl"

Texture2D gFrameTexture : register(TEXTURE_REGISTER_DIFFUSE);
Texture2D gLuminanceTexture : register(TEXTURE_REGISTER_EXTRA);

cbuffer TonemappingConstants : register(CBUFFER_REGISTER_PIXEL)
{
	unsigned int gMipMapLevel;
};


/*

float3 ToneMap(float3 color, float avgLuminance, float threshold, out float exposure)
{
	float pixelLuminance = CalcLuminance(color);
	color = CalcExposedColor(color, avgLuminance, threshold, exposure);

	color = ToneMapFilmicU2(color);
}

return color;
}

// Applies exposure and tone mapping to the input, and combines it with the
// results of the bloom pass
void Composite(in PSInput input,
	out float4 outputColor : SV_Target0,
	out float4 outputExposure : SV_Target1)
{
	// Tone map the primary input
	float avgLuminance = GetAvgLuminance(InputTexture1, input.TexCoord);
	float3 color = InputTexture0.Sample(PointSampler, input.TexCoord).rgb;
	float exposure = 0;
	color = ToneMap(color, avgLuminance, 0, exposure);

	// Sample the bloom
	float3 bloom = InputTexture2.Sample(LinearSampler, input.TexCoord).rgb;
	bloom = bloom * BloomMagnitude;

	// Add in the bloom
	color = color + bloom;

	outputColor = float4(color, 1.0f);
	// outputExposure = (exposure + 10.0f) / 20.0f;
	outputExposure = avgLuminance;
	outputExposure.a = 1.0f;
}

*/


float3 TonemappingU2(const float3 rgb)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;

	return ((rgb * (A * rgb + C * B) + D * E) / (rgb * (A * rgb + B) + D * F)) - E / F;
}

float4 ps_main(float4 position: SV_Position) : SV_Target0
{
	const float3 rgb = gFrameTexture[uint2(position.xy)].rgb;
	const float3 tonemappedRGB = TonemappingU2(rgb);

	return float4(tonemappedRGB, 1.0);
}

#endif