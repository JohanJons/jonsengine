#ifndef CONSTANTS_HLSL
#define CONSTANTS_HLSL

// GBUFFER shaders
#ifdef GBUFFER_CONSTANTS
cbuffer GBufferConstants : register(b0)
{
    float4x4 gWVPMatrix;
    float4x4 gWorldMatrix;
    float gTextureTilingFactor;
    bool gHasDiffuseTexture;
};

struct GBufferVSIn
{
    float3 mPosition : POSITION;
    float2 mTexcoord : TEXCOORD;
};

struct GBufferVSOut
{
    float4 mPosition : SV_POSITION;
    float4 mWorldPosition : WORLD_POSITION;
    float2 mTexcoord : TEXCOORD;
};

struct GBufferPSOut
{
    float4 mDiffuse : SV_Target0;
    float4 mWorldPosition : SV_Target1;
};
#endif

// ambient pixel shader
#ifdef AMBIENT_CONSTANTS
cbuffer AmbientConstants : register(b0)
{
    float4 gAmbientLight;
    float2 gScreenSize;
};
#endif

#endif