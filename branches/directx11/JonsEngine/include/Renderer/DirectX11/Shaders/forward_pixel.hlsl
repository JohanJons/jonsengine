Texture2D gDiffuseTexture : register(t0);
SamplerState gSampler : register(s0);

float4 main(float4 position : POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    return float4(1.0, 1.0, 1.0, 0.0);// gDiffuseTexture.sample();
}