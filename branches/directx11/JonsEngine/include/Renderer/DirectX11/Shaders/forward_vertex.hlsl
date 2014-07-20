cbuffer VertexConstants : register(b0)
{
    float4x4 gWVPMatrix;
    float4 gColor;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VOut main(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VOut output;

    output.position = mul(position, gWVPMatrix);
    output.texcoord = texcoord;

    return output;
}