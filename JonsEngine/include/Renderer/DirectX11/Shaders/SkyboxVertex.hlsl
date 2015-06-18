#ifndef SKYBOX_VERTEX_HLSL
#define SKYBOX_VERTEX_HLSL

cbuffer TransformConstants : register(CBUFFER_REGISTER_VERTEX)
{
    float4x4 gInvVPMatrix;
};

struct GBufferVSOut
{
    float4 mPosition : SV_POSITION;
    float4 mWorldPosition : WORLDPOS;
};


GBufferVSOut vs_main(uint VertexID: SV_VertexID)
{
    GBufferVSOut output;
    output.mPosition = float4(float2(((VertexID << 1) & 2) * 2.0f, (VertexID == 0) * -4.0f) + float2(-1.0f, 1.0f), 1.0f, 1.0f);
    output.mWorldPosition = mul(gInvVPMatrix, output.mPosition);

    return output;
}

#endif