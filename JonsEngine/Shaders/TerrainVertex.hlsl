#ifndef TERRAIN_VERTEX_HLSL
#define TERRAIN_VERTEX_HLSL

#define TERRAIN_DEBUG_NORMAL 1

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
	bool doMorphing = LODlevel > 0;

	float morphLerpK = 0.0f;
	float2 texcoord;
	float4 outWorldPos = worldPos;
	if ( doMorphing )
	{
		float4 preMorphPos = worldPos;
		float2 preMorphTexcoord = GetTextureCoordinates( preMorphPos.xyz );
		preMorphPos.y += SampleHeightmap( preMorphTexcoord ) * gHeightModifier;

		float scaleX = worldTransform[ 0 ][ 0 ];
		float scaleZ = worldTransform[ 2 ][ 2 ];

		float cameraDistanceToVertex = distance( preMorphPos.xyz, gWorldEyePos );
		float2 morphConstants = gLODMorphConstants.Load( LODlevel );

		morphLerpK  = 1.0f - clamp( morphConstants.x - cameraDistanceToVertex * morphConstants.y, 0.0f, 1.0f );

		float meshSize = ( float) gTerrainMeshSize;
		float gridDimHalf = meshSize / 2.0f;
		float oneOverGridDim = 2.0f / meshSize;

		float2 fracPart = ( frac( input.mPosition.xz * float2( gridDimHalf, gridDimHalf ) ) * float2( oneOverGridDim, oneOverGridDim ) ) * float2( scaleX, scaleZ );
		preMorphPos.xz = preMorphPos.xz - ( fracPart * morphLerpK );

		float2 postMorphTexcoord = GetTextureCoordinates( preMorphPos.xyz );
		
		texcoord = postMorphTexcoord;
		outWorldPos.xz = preMorphPos.xz;
		outWorldPos.y += SampleHeightmap( postMorphTexcoord ) * gHeightModifier;
	}
	else
	{
		texcoord = GetTextureCoordinates( outWorldPos.xyz );
		outWorldPos.y += SampleHeightmap( texcoord ) * gHeightModifier;
	}

	const int2 offset = 0;
	const int mipmap = 0;
	//float3 normal = gNormalMap.SampleLevel( gLinearSampler, texcoord, mipmap, offset ).rgb;
	//int2 inputz = texcoord * ( worldPos.xz - gWorldMin ) / ( gWorldMax - gWorldMin );
	//float3 normal = SobelFilter( gHeightmap, int3( inputz, 0) );
	//normal *= 2.0;
	//normal -= 1.0;


	/*float4 h;
	h[0] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0,-1) ).r * gHeightModifier;
	h[1] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2(-1, 0) ).r * gHeightModifier;
	h[2] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 0) ).r * gHeightModifier;
	h[3] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, 1) ).r * gHeightModifier;
	float3 n;
	n.z = h[0] - h[3];
	n.x = h[1] - h[2];
	n.y = 2;

	n *= 2.0;
	n -= 1.0;

	float3 normal = normalize(n); */
	float3 normal = float3( 0, 1, 0 );
#ifdef TERRAIN_NORMAL_SIMPLE
	float3 n;
	n = gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0,-1) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2(-1, 0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, 1) ).xyz;
	n /= 4;

	n *= 2.0;
	n -= 1.0;

	normal = normalize(n);
#endif

#ifdef TERRAIN_NORMAL_BETTER
	/*float3 n;
	n = gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0,-1) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2(-1, 0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, 1) ).xyz;
	n /= 4;

	n *= 2.0;
	n -= 1.0;

	normal = normalize(n); */

	float4 h;
	h[0] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, -1) ).r * gHeightModifier;
	h[1] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( -1, 0 ) ).r * gHeightModifier;
	h[2] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 0) ).r * gHeightModifier;
	h[3] = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, 1) ).r * gHeightModifier;
	float3 n;
	n.z = h[0] - h[3];
	n.x = h[1] - h[2];
	n.y = 1;

	normal = normalize(n); 

#endif

#ifdef TERRAIN_NORMAL_BEST
	/*float3 n;
	n = gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0,0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0,-1) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2(-1, 0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 0) ).xyz;
	n += gNormalMap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, 1) ).xyz;
	n /= 5;

	n *= 2.0;
	n -= 1.0;

	normal = normalize(n); */

	float zb = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, -1) ).r * gHeightModifier;
	float zc = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 0) ).r * gHeightModifier;
	float zd = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 1, 1) ).r * gHeightModifier;
	float ze = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( 0, 1) ).r * gHeightModifier;
	float zf = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( -1, 0) ).r * gHeightModifier;
	float zg = gHeightmap.SampleLevel(gAnisotropicSampler, texcoord, mipmap, float2( -1, -1) ).r * gHeightModifier;

	float x = 2 * zf + zc + zg - zb - 2 * zc - zd;
	float z = 2 * zb + zc + zg - zd - 2 * ze - zf;
	float y = 2.0f;

	normal = float3( x, y, z );
	normal = normalize( normal );
#endif

	VertexOut ret;
	ret.mPosition = mul( gFrameViewProj, outWorldPos );
#ifdef TERRAIN_DEBUG_NORMAL
	ret.mWorldNormal = normal;
#endif
	ret.mNormal = mul( ( float3x3 )gFrameView, normal );
	ret.mNormal = normalize( ret.mNormal );
	ret.mLOD = LODlevel;
	ret.mMorph = morphLerpK;

	return ret;
}

#endif