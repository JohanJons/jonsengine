#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

StructuredBuffer<float> gLODRanges : register (SBUFFER_REGISTER_EXTRA_2);

#define MIN_TESSELLATION 4.0f
#define MIN_COPLANARITY 0.1f

uint GetLOD( float3 worldPos )
{
	float dist = distance( gWorldEyePos, worldPos );
	for ( uint LOD = gNumLODs - 1; LOD > 0; --LOD )
	{
		if ( dist <= gLODRanges[ LOD ] )
			return LOD;
	}

	return 0;
}

float CalculateTessellation( uint centerLOD, float3 edgeVertex1, float3 edgeVertex2 )
{
	float3 midVertex = ( edgeVertex1 + edgeVertex2 ) / 2;
	uint edgeLOD = GetLOD( midVertex );
	if ( edgeLOD == centerLOD )
	{
		return 8;
	}
	else if ( centerLOD > edgeLOD )
	{
		return 16;
	}
	else
	{
		return 8;
	}

	/*uint sideLOD = GetLOD( sideCenterM );
	if ( centerLOD == sideLOD )
	{
		return 8;
	}
	else if ( centerLOD < sideLOD )
	{
		return 16;
	}
	else
	{
		return 8;
	}*/

	/*float3 bottomCenterM = inputVertices[ 4 ].mWorldPosition;
	uint LOD = GetLOD( bottomCenterM );
	if ( centerLOD == LOD )
	{

	}


	float3 bottomCenterS, bottomCenterL;
	ComputeMidpoints( BL, bottomCenterM, bottomCenterS, bottomCenterL );

	float distM = distance( gWorldEyePos, bottomCenterM );*/
}

float3 ComputeMidpoints( float3 vertex, float3 midpointM, out float3 midpointS, out float3 midpointL )
{
	float3 diff = midpointM - vertex;
	
	midpointS = vertex + ( diff / 2 );
	midpointL = midpointM + diff;
}

float GetScreenSpaceTessellationScale( float2 screenPos1, float2 screenPos2 )
{
	float screenLength = distance( screenPos1, screenPos2 );
	float scale = screenLength / (float) gTerrainPrimitveLength;
	scale = saturate( scale );

	return scale;
}

float3 ComputePatchMidpoint( float3 corner1, float3 corner2, float3 corner3, float3 corner4 )
{
	return ( corner1 + corner2 + corner3 + corner4 ) / 4.0f;
}

float CalculateTessellationfactor( float midPatchDistance, float3 q1, float3 q2, float3 q3, float3 q4 )
{
	float3 worldPatchMidpoint = ComputePatchMidpoint( q1, q2, q3, q4 );
	float dist = distance( gWorldEyePos, worldPatchMidpoint );
	if ( dist <= midPatchDistance )
	{
		return 64;
	}
	else
	{
		return 2;
	} 

	/*
	// coplanarity part
	float3 worldPatchMidpoint = ComputePatchMidpoint( q1, q2, q3, q4 );
	float coplanarity = GetCoplanarity( worldPatchMidpoint.xz );
	coplanarity *= gTerrainCoplanarityScale;

	float tessCoplanarity = gTerrainTessellationMax * gTerrainCoplanarityTessellationRatio;
	tessCoplanarity *= coplanarity;
	tessCoplanarity = max( tessCoplanarity, MIN_COPLANARITY );

	// screen space part
	float2 screenPos1 = GetScreenSpacePosition( q1 );
	float2 screenPos2 = GetScreenSpacePosition( q2 );
	float2 screenPos3 = GetScreenSpacePosition( q3 );
	float2 screenPos4 = GetScreenSpacePosition( q4 );

	float screenSpaceScale = max( GetScreenSpaceTessellationScale( screenPos1, screenPos3 ), GetScreenSpaceTessellationScale( screenPos2, screenPos4 ) );
	float tessScreenSpace = gTerrainTessellationMax * ( 1.0f - gTerrainCoplanarityTessellationRatio );
	tessScreenSpace *= screenSpaceScale;

	float finalTessellation = tessCoplanarity + tessScreenSpace;
	finalTessellation = max( finalTessellation, MIN_TESSELLATION );

	return finalTessellation;
	*/
}

PatchTess PatchHS( InputPatch<VertexOut, 8> inputVertices )
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
	float3 BL = inputVertices[ 0 ].mWorldPosition;
	float3 BR = inputVertices[ 1 ].mWorldPosition;
	float3 TR = inputVertices[ 2 ].mWorldPosition;
	float3 TL = inputVertices[ 3 ].mWorldPosition;
	float3 centerPatchMidPoint = ComputePatchMidpoint( BL, BR, TR, TL );
	uint centerLOD = GetLOD( centerPatchMidPoint );

	float3 bottomCenterM = inputVertices[ 4 ].mWorldPosition;
	float3 rightCenterM = inputVertices[ 5 ].mWorldPosition;
	float3 topCenterM = inputVertices[ 6 ].mWorldPosition;
	float3 leftCenterM = inputVertices[ 7 ].mWorldPosition;

	//if ( distM <  )

	/*float midPatchTessFactor = CalculateTessellationfactor( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition );
	float edgePatchTessFactors[] = {
		CalculateTessellationfactor( inputVertices[ 11 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition, inputVertices[ 10 ].mWorldPosition ),
		CalculateTessellationfactor( inputVertices[ 4 ].mWorldPosition, inputVertices[ 5 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition ),
		CalculateTessellationfactor( inputVertices[ 1 ].mWorldPosition, inputVertices[ 6 ].mWorldPosition, inputVertices[ 7 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition ),
		CalculateTessellationfactor( inputVertices[ 3 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 8 ].mWorldPosition, inputVertices[ 9 ].mWorldPosition )
	};*/
	float midPatchTessFactor = 16.0f;
	float edgePatchTessFactors[] = {
		CalculateTessellation( centerLOD, BL, BR ),
		CalculateTessellation( centerLOD, BR, TR ),
		CalculateTessellation( centerLOD, TR, TL ),
		CalculateTessellation( centerLOD, TL, BL )
		//CalculateTessellation( centerLOD, BL, bottomCenterM ),
		//CalculateTessellation( centerLOD, BR, rightCenterM ),
		//CalculateTessellation( centerLOD, TR, topCenterM ),
		//CalculateTessellation( centerLOD, TL, leftCenterM )
		//CalculateTessellationfactor( dist, inputVertices[ 11 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition, inputVertices[ 10 ].mWorldPosition ),
		//CalculateTessellationfactor( dist, inputVertices[ 4 ].mWorldPosition, inputVertices[ 5 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition ),
		//CalculateTessellationfactor( dist, inputVertices[ 1 ].mWorldPosition, inputVertices[ 6 ].mWorldPosition, inputVertices[ 7 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition ),
		//CalculateTessellationfactor( dist, inputVertices[ 3 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 8 ].mWorldPosition, inputVertices[ 9 ].mWorldPosition )
	};

	patch.mEdgeTess[ 0 ] = min( midPatchTessFactor, edgePatchTessFactors[ 3 ] );
	patch.mEdgeTess[ 1 ] = min( midPatchTessFactor, edgePatchTessFactors[ 0 ] );
	patch.mEdgeTess[ 2 ] = min( midPatchTessFactor, edgePatchTessFactors[ 1 ] );
	patch.mEdgeTess[ 3 ] = min( midPatchTessFactor, edgePatchTessFactors[ 2 ] );
	
	patch.mInsideTess[ 0 ] = midPatchTessFactor;
	patch.mInsideTess[ 1 ] = midPatchTessFactor;
#endif

	return patch;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchHS")]
HullOut hs_main( InputPatch<VertexOut, 8> verticeData, uint index : SV_OutputControlPointID )
{
	HullOut ret;

	ret.mWorldPosition = verticeData[ index ].mWorldPosition;
	ret.mTexcoord = verticeData[ index ].mTexcoord;

	return ret;
}

#endif