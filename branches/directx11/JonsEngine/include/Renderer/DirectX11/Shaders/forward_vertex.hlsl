cbuffer VertexConstants : register (b0)
{
    float4x4 gWVPMatrix;
    float4 gColor;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut main(float4 position : POSITION)
{
    VOut output;

    output.position = mul(position, gWVPMatrix);
    output.color = gColor;

    return output;
}