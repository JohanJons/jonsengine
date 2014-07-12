struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut main(float4 position : POSITION)
{
    VOut output;

    output.position = position;
    output.color = float4(1.0, 0.0, 0.0, 0.0);

    return output;
}