cbuffer VertexConstants : register(b0)
{
    float4x4 gWVPMatrix;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VOut main(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VOut output;

    output.position = mul(gWVPMatrix, position);
    output.texcoord = 64.0 * texcoord;

    return output;
}