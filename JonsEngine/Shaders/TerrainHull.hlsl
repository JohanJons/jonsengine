#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"
#include "Common.hlsl"

StructuredBuffer<float4> gTessEdgeFactors : register (SBUFFER_REGISTER_EXTRA_2);

#define MIN_TESSELLATION 4.0f
#define MIN_COPLANARITY 0.1f

float CalculateTessellation( float edgeFactor )
{
	float tessellation = 32.0f;
	tessellation *= edgeFactor;

	return tessellation;
}

/*float GetScreenSpaceTessellationScale( float2 screenPos1, float2 screenPos2 )
{
	float screenLength = distance( screenPos1, screenPos2 );
	float scale = screenLength / (float) gTerrainPrimitveLength;
	scale = saturate( scale );

	return scale;
}*/

float3 ComputePatchMidpoint( float3 corner1, float3 corner2, float3 corner3, float3 corner4 )
{
	return ( corner1 + corner2 + corner3 + corner4 ) / 4.0f;
}

PatchTess PatchHS( InputPatch<VertexOut, 4> inputVertices )
{
	PatchTess patch;

	uint instanceID = 0;// inputVertices[ 0 ].mInstanceID;

#if TERRAIN_DEBUG_COPLANARITY
	patch.mEdgeTess[ 0 ] = 32;
	patch.mEdgeTess[ 1 ] = patch.mEdgeTess[ 0 ];
	patch.mEdgeTess[ 2 ] = patch.mEdgeTess[ 0 ];
	patch.mEdgeTess[ 3 ] = patch.mEdgeTess[ 0 ];

	patch.mInsideTess[ 0 ] = patch.mEdgeTess[ 0 ];
	patch.mInsideTess[ 1 ] = patch.mEdgeTess[ 0 ];
#else
	float leftFactor = gTessEdgeFactors[ instanceID ].x;
	float bottomFactor = gTessEdgeFactors[ instanceID ].y;
	float rightFactor = gTessEdgeFactors[ instanceID ].z;
	float topFactor = gTessEdgeFactors[ instanceID ].w;

	float edgePatchTessFactors[] = {
		CalculateTessellation( leftFactor ),
		CalculateTessellation( bottomFactor ),
		CalculateTessellation( rightFactor ),
		CalculateTessellation( topFactor )
	};

	patch.mEdgeTess[ 0 ] = edgePatchTessFactors[ 0 ];
	patch.mEdgeTess[ 1 ] = edgePatchTessFactors[ 1 ];
	patch.mEdgeTess[ 2 ] = edgePatchTessFactors[ 2 ];
	patch.mEdgeTess[ 3 ] = edgePatchTessFactors[ 3 ];
	
	float midPatchTessFactor = 32.0f;
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
HullOut hs_main( InputPatch<VertexOut, 4> verticeData, uint index : SV_OutputControlPointID )
{
	HullOut ret;

	ret.mWorldPosition = float3( 1, 1, 1 );
	ret.mTexcoord = float2( 1, 1 );

	return ret;
}

#endif