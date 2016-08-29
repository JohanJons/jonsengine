#ifndef TONEMAPPING_VERTEX_HLSL
#define TONEMAPPING_VERTEX_HLSL

float4 ps_main(uint VertexID: SV_VertexID) : SV_Position
{
    return float4(1.0, 1.0, 1.0, 1.0);
}

#endif