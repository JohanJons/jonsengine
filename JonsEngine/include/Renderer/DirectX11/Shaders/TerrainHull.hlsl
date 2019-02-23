#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"

float3 ComputePatchMidpoint( float3 corner1, float3 corner2, float3 corner3, float3 corner4 )
{
	return ( corner1 + corner2 + corner3 + corner4 ) / 4.0f;
}

float3 ComputeEdgeMidpoint( float3 point1, float3 point2 )
{
	return ( point1 + point2 ) / 2.0f;
}

float CalculateTessellationfactor( float3 worldPatchMidpoint )
{
	float cameraToPatchDistance = distance( gWorldEyePos, worldPatchMidpoint );
	float scaledDistance = ( cameraToPatchDistance - gMinZ ) / ( gMaxZ - gMinZ );
	scaledDistance = clamp( scaledDistance, 0.0f, 1.0f );

	return pow( 2, lerp( 6.0f, 2.0f, scaledDistance ) );
}

PatchTess PatchHS( InputPatch<VertexOut, 4> inputVertices )
{
	PatchTess patch;

	float3 worldPatchMidpoint = ComputePatchMidpoint( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition );
	float3 edgeMidpoints[] = {
		ComputeEdgeMidpoint( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition ),
		ComputeEdgeMidpoint( inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition ),
		ComputeEdgeMidpoint( inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition ),
		ComputeEdgeMidpoint( inputVertices[ 3 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition ),
	};

	float patchTessFactor = CalculateTessellationfactor( worldPatchMidpoint );
	float edgeTessFactors[] = {
		CalculateTessellationfactor( edgeMidpoints[ 0 ] ),
		CalculateTessellationfactor( edgeMidpoints[ 1 ] ),
		CalculateTessellationfactor( edgeMidpoints[ 2 ] ),
		CalculateTessellationfactor( edgeMidpoints[ 3 ] )
	};

	patch.mEdgeTess[ 0 ] = min( patchTessFactor, edgeTessFactors[ 0 ] );
	patch.mEdgeTess[ 1 ] = min( patchTessFactor, edgeTessFactors[ 1 ] );
	patch.mEdgeTess[ 2 ] = min( patchTessFactor, edgeTessFactors[ 2 ] );
	patch.mEdgeTess[ 3 ] = min( patchTessFactor, edgeTessFactors[ 3 ] );
	
	patch.mInsideTess[ 0 ] = patchTessFactor;
	patch.mInsideTess[ 1 ] = patchTessFactor;

	return patch;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchHS")]
HullOut hull_main( InputPatch<VertexOut, 4> verticeData, uint index : SV_OutputControlPointID )
{
	HullOut ret;

	ret.mWorldPosition = verticeData[ index ].mWorldPosition;
	ret.mTexcoord = verticeData[ index ].mTexcoord;

	return ret;
}

#endif