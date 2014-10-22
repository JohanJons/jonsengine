#include "Constants.h"

#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 25
#define FXAA_GREEN_AS_LUMA 1

#include "FXAA.hlsl"


cbuffer FXAACBuffer : register(CBUFFER_REGISTER_PIXEL)
{
    float2 gScreenSizeReciprocal;
};

Texture2D gFinalTexture : register(TEXTURE_REGISTER_EXTRA);
SamplerState gPointSampler : register(SAMPLER_REGISTER_POINT);

float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    FxaaTex InputFXAATex = { gPointSampler, gFinalTexture };

    float2 coords = float2(position.x * gScreenSizeReciprocal.x, position.y * gScreenSizeReciprocal.y);

    //return gFinalTexture.Sample(gPointSampler, coords);
    return FxaaPixelShader(
        coords,							        // FxaaFloat2 pos,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsolePosPos,
        InputFXAATex,							// FxaaTex tex,
        InputFXAATex,							// FxaaTex fxaaConsole360TexExpBiasNegOne,
        InputFXAATex,							// FxaaTex fxaaConsole360TexExpBiasNegTwo,
        gScreenSizeReciprocal,				    // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        0.75f,									// FxaaFloat fxaaQualitySubpix,
        0.166f,									// FxaaFloat fxaaQualityEdgeThreshold,
        0.0833f,								// FxaaFloat fxaaQualityEdgeThresholdMin,
        0.0f,									// FxaaFloat fxaaConsoleEdgeSharpness,
        0.0f,									// FxaaFloat fxaaConsoleEdgeThreshold,
        0.0f,									// FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)		// FxaaFloat fxaaConsole360ConstDir,
    );
}