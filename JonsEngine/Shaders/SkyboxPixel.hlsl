#ifndef SKYBOX_PIXEL_HLSL
#define SKYBOX_PIXEL_HLSL

#include "Constants.hlsl"
#include "SkyboxVertex.hlsl"

TextureCube gSkybox : register(TEXTURE_REGISTER_EXTRA);

float4 ps_main(GBufferVSOut input) : SV_Target0
{
    return gSkybox.Sample( gAnisotropicSampler, input.mTexcoord );
}

#endif