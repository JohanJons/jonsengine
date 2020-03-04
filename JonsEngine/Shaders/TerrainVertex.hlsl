#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#include "TerrainCommon.hlsl"

struct VertexIn
{
	float3 mPosition : POSITION;
	uint mInstanceID : SV_InstanceID;
};

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );

StructuredBuffer<float2> gLODMorphConstants : register ( SBUFFER_REGISTER_EXTRA );
StructuredBuffer<float4x4> gTerrainWorldTransforms : register ( SBUFFER_REGISTER_EXTRA_2 );
StructuredBuffer<uint> gTerrainLODLevels : register ( SBUFFER_REGISTER_EXTRA_3 );

float2 GetTextureCoordinates( float3 worldPos )
{
	float2 texcoord = ( worldPos.xz - gWorldMin ) / ( gWorldMax - gWorldMin );
	texcoord = clamp( texcoord, 0.0f, 1.0f );

	return texcoord;
}

float SampleHeightmap( float2 uv )
{
	int mipmap = 0;
	int offset = 0;

	return gHeightmap.SampleLevel( gLinearSampler, uv, mipmap, offset ).r;
}

VertexOut vs_main(VertexIn input)
{
	const uint transformIndex = gTransformOffset + input.mInstanceID;
	// silly that we have to transpose this...
	const float4x4 worldTransform = transpose( gTerrainWorldTransforms.Load( transformIndex ) );
	const uint LODlevel = gTerrainLODLevels.Load( transformIndex );

	const float4 worldPos = mul( worldTransform, float4( input.mPosition, 1 ) );

	float4 preMorphPos = worldPos;
	float2 preMorphTexcoord = GetTextureCoordinates( preMorphPos.xyz );
	preMorphPos.y += SampleHeightmap( preMorphTexcoord ) * gHeightModifier;

	float scaleX = worldTransform[ 0 ][ 0 ];
	float scaleZ = worldTransform[ 2 ][ 2 ];

	float cameraDistanceToVertex = distance( preMorphPos.xyz, gWorldEyePos );
	float2 morphConstants = gLODMorphConstants.Load( LODlevel );

	float morphLerpK  = 1.0f - clamp( morphConstants.x - cameraDistanceToVertex * morphConstants.y, 0.0f, 1.0f );

	float meshSize = ( float) gTerrainMeshSize;
	float gridDimHalf = meshSize / 2.0f;
	float oneOverGridDim = 2.0f / meshSize;

	float2 fracPart = ( frac( input.mPosition.xz * float2( gridDimHalf, gridDimHalf ) ) * float2( oneOverGridDim, oneOverGridDim ) ) * float2( scaleX, scaleZ );
	preMorphPos.xz = preMorphPos.xz - ( fracPart * morphLerpK );

	float4 postMorphPos = preMorphPos;
	float2 postMorphTexcoord = GetTextureCoordinates( preMorphPos.xyz );
	postMorphPos.y += SampleHeightmap( postMorphTexcoord ) * gHeightModifier;

	const int2 offset = 0;
	const int mipmap = 0;
	float3 normal = gNormalMap.SampleLevel( gLinearSampler, postMorphTexcoord, mipmap, offset ).rgb;
	normal *= 2.0;
	normal -= 1.0;

	VertexOut ret;
	ret.mPosition = mul( gFrameViewProj, postMorphPos );
	ret.mNormal = mul( ( float3x3 )gFrameView, normal );
	ret.mNormal = normalize( ret.mNormal );
	ret.mLOD = LODlevel;
	ret.mMorph = morphLerpK;

	return ret;
}

#endif