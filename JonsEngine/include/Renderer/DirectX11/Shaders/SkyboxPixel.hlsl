#ifndef SKYBOX_PIXEL_HLSL
#define SKYBOX_PIXEL_HLSL

Texture2D gSkybox : register(TEXTURE_REGISTER_EXTRA);
SamplerState gSampler : register(SAMPLER_REGISTER_ANISOTROPIC);


float4 ps_main(GBufferVSOut input) : SV_Target0
{
    return gSkybox.Sample(gSampler, input.mWorldPosition.xyz);
}

#endif