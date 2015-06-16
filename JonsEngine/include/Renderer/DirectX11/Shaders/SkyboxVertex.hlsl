#ifndef SKYBOX_VERTEX_HLSL
#define SKYBOX_VERTEX_HLSL

float4 vs_main(uint VertexID: SV_VertexID) : SV_Position
{
    return float4(float2(((VertexID << 1) & 2) * 2.0f, (VertexID == 0) * -4.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
}

#endif