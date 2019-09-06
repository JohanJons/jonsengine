#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

StructuredBuffer<float> gLODRanges : register (SBUFFER_REGISTER_EXTRA_2);
StructuredBuffer<float4> gTessEdgeFactors : register (SBUFFER_REGISTER_EXTRA_3);

/*
float dmin = 0;
float  r2 = r * r;
for( i = 0; i < n; i++ ) {
	if( C[i] < Bmin[i] ) dmin += SQR(C[i] - Bmin[i] ); else
		if( C[i] > Bmax[i] ) dmin += SQR( C[i] - Bmax[i] );     
}
if( dmin <= r2 ) return( TRUE );
*/



#define MIN_TESSELLATION 4.0f
#define MIN_COPLANARITY 0.1f

bool SphereIntersects( float3 sphereCenter, float radius, float3 aabbMin, float3 aabbMax )
{
	float dmin = 0;
	float radiusSquared = radius * radius;

	for( int i = 0; i < 3; ++i )
	{
		if( sphereCenter[i] < aabbMin[i] )
			dmin += pow( sphereCenter[i] - aabbMin[i], 2 );
		else if( sphereCenter[i] > aabbMax[i] )
			dmin += pow( sphereCenter[i] - aabbMax[i], 2 );
	}

	return dmin <= radiusSquared;
}

/*uint GetLOD( float3 worldPos )
{
	float dist = distance( gWorldEyePos, worldPos );
	for ( uint LOD = gNumLODs - 1; LOD > 0; --LOD )
	{
		if ( dist <= gLODRanges[ LOD ] )
			return LOD;
	}

	return 0;
}*/

uint GetLOD( float3 minVertex, float3 maxVertex )
{
	for ( uint LOD = gNumLODs - 1; LOD > 0; --LOD )
	{
		float dist = gLODRanges[ LOD ];
		if ( SphereIntersects( gWorldEyePos, dist, minVertex, maxVertex ) )
		{
			return LOD;
		}
	}

	return 0;
}

/*float CalculateTessellation( uint centerLOD, float3 minVertex, float3 maxVertex )
{
	float tessellation = 4.0f;

	uint nextLOD = centerLOD + 1;
	nextLOD = clamp( nextLOD, 0, gNumLODs - 1 );
	if ( nextLOD != centerLOD )
	{
		float dist = gLODRanges[ nextLOD ];
		if ( SphereIntersects( gWorldEyePos, dist, minVertex, maxVertex ) )
		{
			tessellation = 8.0f;
		}
	}

	return tessellation;
}*/

float CalculateTessellation( float edgeFactor )
{
	float tessellation = 8.0f;
	//if ( edgeFactor < 2.0f )
	{
		tessellation *= edgeFactor;
	}

	return tessellation;
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

PatchTess PatchHS( InputPatch<VertexOut, 8> inputVertices )
{
	PatchTess patch;

	uint instanceID = inputVertices[ 0 ].mInstanceID;

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
	//float3 centerPatchMidPoint = ComputePatchMidpoint( BL, BR, TR, TL );
	//uint centerLOD = GetLOD( centerPatchMidPoint );
	uint centerLOD = GetLOD( BL, TR );

	float3 bottomQuadVertex = inputVertices[ 4 ].mWorldPosition;
	float3 rightQuadVertex = inputVertices[ 5 ].mWorldPosition;
	float3 topQuadVertex = inputVertices[ 6 ].mWorldPosition;
	float3 leftQuadVertex = inputVertices[ 7 ].mWorldPosition;

	float leftFactor = gTessEdgeFactors[ instanceID ].x;
	float bottomFactor = gTessEdgeFactors[ instanceID ].y;
	float rightFactor = gTessEdgeFactors[ instanceID ].z;
	float topFactor = gTessEdgeFactors[ instanceID ].w;

	float midPatchTessFactor = 64.0f;
	float edgePatchTessFactors[] = {
		CalculateTessellation( leftFactor ),
		CalculateTessellation( bottomFactor ),
		CalculateTessellation( rightFactor ),
		CalculateTessellation( topFactor )
		//CalculateTessellation( centerLOD, BL, bottomCenterM ),
		//CalculateTessellation( centerLOD, BR, rightCenterM ),
		//CalculateTessellation( centerLOD, TR, topCenterM ),
		//CalculateTessellation( centerLOD, TL, leftCenterM )
		//CalculateTessellationfactor( dist, inputVertices[ 11 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition, inputVertices[ 10 ].mWorldPosition ),
		//CalculateTessellationfactor( dist, inputVertices[ 4 ].mWorldPosition, inputVertices[ 5 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition ),
		//CalculateTessellationfactor( dist, inputVertices[ 1 ].mWorldPosition, inputVertices[ 6 ].mWorldPosition, inputVertices[ 7 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition ),
		//CalculateTessellationfactor( dist, inputVertices[ 3 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 8 ].mWorldPosition, inputVertices[ 9 ].mWorldPosition )
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