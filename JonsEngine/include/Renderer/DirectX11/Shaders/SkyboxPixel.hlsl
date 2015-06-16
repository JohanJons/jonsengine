#ifndef SKYBOX_PIXEL_HLSL
#define SKYBOX_PIXEL_HLSL

Texture2D gSkybox : register(TEXTURE_REGISTER_EXTRA);


float4 ps_main() : SV_Target0
{
    return float4(0.0, 1.0, 0.0, 0.0);
}

#endif