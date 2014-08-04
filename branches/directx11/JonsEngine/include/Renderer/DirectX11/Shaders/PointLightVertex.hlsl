#ifndef POINT_LIGHT_VERTEX_HLSL
#define POINT_LIGHT_VERTEX_HLSL

cbuffer PointLightConstants : register(b0)
{
    float4x4 gLightWVPMatrix;
    float4 gLightColor;
    float4 gLightPosition;
    float2 gScreenSize;
    float gLightIntensity;
    float gMaxDistance;
};


float4 vs_main(float3 position : POSITION) : SV_Position
{
    return mul(gLightWVPMatrix, float4(position, 1.0f));
}

#endif