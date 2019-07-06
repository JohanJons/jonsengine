#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

float GetScreenSpaceTessellationFactor( float2 screenPos1, float2 screenPos2 )
{
	float screenLength = distance( screenPos1, screenPos2 );
	float scale = screenLength / (float) 16;
	scale = saturate( scale );

	return scale * gTerrainTessellationMax;
}

float3 ComputePatchMidpoint( float3 corner1, float3 corner2, float3 corner3, float3 corner4 )
{
	return ( corner1 + corner2 + corner3 + corner4 ) / 4.0f;
}

float CalculateTessellationfactor( float3 q1, float3 q2, float3 q3, float3 q4 )
{
	float2 screenPos1 = GetScreenSpacePosition( q1 );
	float2 screenPos2 = GetScreenSpacePosition( q2 );
	float2 screenPos3 = GetScreenSpacePosition( q3 );
	float2 screenPos4 = GetScreenSpacePosition( q4 );

	float tess1 = GetScreenSpaceTessellationFactor( screenPos1, screenPos3 );
	float tess2 = GetScreenSpaceTessellationFactor( screenPos2, screenPos4 );

    //float cameraToPatchDistance = distance( gWorldEyePos, worldPatchMidpoint );
	//float scaledDistance = ( cameraToPatchDistance - gMinMaxZ.x ) / ( gMinMaxZ.y - gMinMaxZ.x );
	//scaledDistance = clamp( scaledDistance, 0.0f, 1.0f );

	float3 worldPatchMidpoint = ComputePatchMidpoint( q1, q2, q3, q4 );
	float coplanarity = 1.0f;// GetCoplanarity( worldPatchMidpoint.xz );

	float tessellation = max( tess1, tess2 );
	tessellation *= coplanarity;

	return tessellation;
	//return pow( 2, lerp( 6.0f, 2.0f, coplanarity ) );
}

PatchTess PatchHS( InputPatch<VertexOut, 12> inputVertices )
{
	PatchTess patch;

#if TERRAIN_DEBUG_COPLANARITY
	patch.mEdgeTess[ 0 ] = 32;
	patch.mEdgeTess[ 1 ] = patch.mEdgeTess[ 0 ];
	patch.mEdgeTess[ 2 ] = patch.mEdgeTess[ 0 ];
	patch.mEdgeTess[ 3 ] = patch.mEdgeTess[ 0 ];

	patch.mInsideTess[ 0 ] = patch.mEdgeTess[ 0 ];
	patch.mInsideTess[ 1 ] = patch.mEdgeTess[ 0 ];
#else
	float midPatchTessFactor = CalculateTessellationfactor( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition );
	float edgePatchTessFactors[] = {
		CalculateTessellationfactor( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 4 ].mWorldPosition, inputVertices[ 5 ].mWorldPosition ),
		CalculateTessellationfactor( inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 6 ].mWorldPosition, inputVertices[ 7 ].mWorldPosition ),
		CalculateTessellationfactor( inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition, inputVertices[ 8 ].mWorldPosition, inputVertices[ 9 ].mWorldPosition ),
		CalculateTessellationfactor( inputVertices[ 3 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition, inputVertices[ 10 ].mWorldPosition, inputVertices[ 11 ].mWorldPosition )
	};

	patch.mEdgeTess[ 0 ] = min( midPatchTessFactor, edgePatchTessFactors[ 0 ] );
	patch.mEdgeTess[ 1 ] = min( midPatchTessFactor, edgePatchTessFactors[ 1 ] );
	patch.mEdgeTess[ 2 ] = min( midPatchTessFactor, edgePatchTessFactors[ 2 ] );
	patch.mEdgeTess[ 3 ] = min( midPatchTessFactor, edgePatchTessFactors[ 3 ] );
	
	patch.mInsideTess[ 0 ] = midPatchTessFactor;
	patch.mInsideTess[ 1 ] = midPatchTessFactor;
#endif

	return patch;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchHS")]
HullOut hs_main( InputPatch<VertexOut, 12> verticeData, uint index : SV_OutputControlPointID )
{
	HullOut ret;

	ret.mWorldPosition = verticeData[ index ].mWorldPosition;
	ret.mTexcoord = verticeData[ index ].mTexcoord;

	return ret;
}

#endif