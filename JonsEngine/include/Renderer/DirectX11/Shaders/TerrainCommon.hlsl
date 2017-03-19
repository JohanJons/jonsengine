#ifndef TERRAIN_COMMON_HLSL
#define TERRAIN_COMMON_HLSL

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

struct VertexOut
{
	float3 mPosition : POSITION;
	float3 mNormal   : NORMAL;
	float2 Tex       : TEXCOORD;
	float TessFactor : TESS;
};

struct DomainOut
{
	float4 PosH     : SV_POSITION;
	float3 PosW     : POSITION;
	float3 NormalW  : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex      : TEXCOORD;
};

struct HullOut
{
	float3 PosW     : POSITION;
	float3 NormalW  : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex      : TEXCOORD;
};

#endif