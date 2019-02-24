#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"

float3 ComputePatchMidpoint( float3 corner1, float3 corner2, float3 corner3, float3 corner4 )
{
	return ( corner1 + corner2 + corner3 + corner4 ) / 4.0f;
}

float CalculateTessellationfactor( float3 worldPatchMidpoint )
{
	float cameraToPatchDistance = distance( gWorldEyePos, worldPatchMidpoint );
	float scaledDistance = ( cameraToPatchDistance - gMinZ ) / ( gMaxZ - gMinZ );
	scaledDistance = clamp( scaledDistance, 0.0f, 1.0f );

	return pow( 2, lerp( 6.0f, 2.0f, scaledDistance ) );
}

PatchTess PatchHS( InputPatch<VertexOut, 12> inputVertices )
{
	PatchTess patch;

	float3 midPatchMidpoint = ComputePatchMidpoint( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition );
	float3 edgePatchMidpoint[] = {
		ComputePatchMidpoint( inputVertices[ 0 ].mWorldPosition, inputVertices[ 1 ].mWorldPosition, inputVertices[ 4 ].mWorldPosition, inputVertices[ 5 ].mWorldPosition ),
		ComputePatchMidpoint( inputVertices[ 1 ].mWorldPosition, inputVertices[ 2 ].mWorldPosition, inputVertices[ 6 ].mWorldPosition, inputVertices[ 7 ].mWorldPosition ),
		ComputePatchMidpoint( inputVertices[ 2 ].mWorldPosition, inputVertices[ 3 ].mWorldPosition, inputVertices[ 8 ].mWorldPosition, inputVertices[ 9 ].mWorldPosition ),
		ComputePatchMidpoint( inputVertices[ 3 ].mWorldPosition, inputVertices[ 0 ].mWorldPosition, inputVertices[ 10 ].mWorldPosition, inputVertices[ 11 ].mWorldPosition )
	};

	float midPatchTessFactor = CalculateTessellationfactor( midPatchMidpoint );
	float edgePatchTessFactors[] = {
		CalculateTessellationfactor( edgePatchMidpoint[ 3 ] ),
		CalculateTessellationfactor( edgePatchMidpoint[ 0 ] ),
		CalculateTessellationfactor( edgePatchMidpoint[ 1 ] ),
		CalculateTessellationfactor( edgePatchMidpoint[ 2 ] )
	};

	patch.mEdgeTess[ 0 ] = min( midPatchTessFactor, edgePatchTessFactors[ 0 ] );
	patch.mEdgeTess[ 1 ] = min( midPatchTessFactor, edgePatchTessFactors[ 1 ] );
	patch.mEdgeTess[ 2 ] = min( midPatchTessFactor, edgePatchTessFactors[ 2 ] );
	patch.mEdgeTess[ 3 ] = min( midPatchTessFactor, edgePatchTessFactors[ 3 ] );
	
	patch.mInsideTess[ 0 ] = midPatchTessFactor;
	patch.mInsideTess[ 1 ] = midPatchTessFactor;

	return patch;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchHS")]
HullOut hull_main( InputPatch<VertexOut, 12> verticeData, uint index : SV_OutputControlPointID )
{
	HullOut ret;

	ret.mWorldPosition = verticeData[ index ].mWorldPosition;
	ret.mTexcoord = verticeData[ index ].mTexcoord;

	return ret;
}

#endif