#ifndef SKYBOX_VERTEX_HLSL
#define SKYBOX_VERTEX_HLSL

cbuffer TransformConstants : register(CBUFFER_REGISTER_VERTEX)
{
    float4x4 gInvViewMatrix;
    float4x4 gInvProjMatrix;
};

struct GBufferVSOut
{
    float4 mPosition : SV_POSITION;
    float3 mTexcoord : WORLDPOS;
};


GBufferVSOut vs_main(uint VertexID: SV_VertexID)
{
    GBufferVSOut output;
    output.mPosition = float4(float2(((VertexID << 1) & 2) * 2.0f, (VertexID == 0) * -4.0f) + float2(-1.0f, 1.0f), 1.0f, 1.0f);

    const float4 viewPos = mul(gInvProjMatrix, output.mPosition);
    output.mTexcoord = mul((float3x3)gInvViewMatrix, viewPos.xyz);

    return output;
}

#endif