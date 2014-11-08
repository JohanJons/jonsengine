#ifndef COMMON_HLSL
#define COMMON_HLSL

float3 reconstructViewPosition(const float depth, const float2 texCoord, const float4x4 invProjMatrix)
{
    float x = texCoord.x * 2 - 1;
    float y = (1 - texCoord.y) * 2 - 1;
    float4 projPos = float4(x, y, depth, 1.0);

    float4 viewPos = mul(invProjMatrix, projPos);

    return viewPos.xyz /= viewPos.w;
}

float3x3 CreateMatrixFromCols(const float3 c0, const float3 c1, const float3 c2)
{
    return float3x3(c0.x, c1.x, c2.x,
                    c0.y, c1.y, c2.y,
                    c0.z, c1.z, c2.z);
}

float4x4 CreateMatrixFromCols(const float4 c0, const float4 c1, const float4 c2, const float4 c3)
{
    return float4x4(c0.x, c1.x, c2.x, c3.x,
                    c0.y, c1.y, c2.y, c3.y,
                    c0.z, c1.z, c2.z, c3.z,
                    c0.w, c1.w, c2.w, c3.w);
}

#endif