#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"

float CalculateTessellationfactor(float3 patchViewPos)
{

}

PatchTess PatchHS(InputPatch<VertexOut, 4> inputVertices)
{
	PatchTess patch;

	patch.mEdgeTess[ 0 ] = 64;
	patch.mEdgeTess[ 1 ] = 64;
	patch.mEdgeTess[ 2 ] = 64;
	patch.mEdgeTess[ 3 ] = 64;
	
	patch.mInsideTess[ 0 ] = 64;
	patch.mInsideTess[ 1 ] = 64;

	return patch;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchHS")]
HullOut hull_main(InputPatch<VertexOut, 4> verticeData, uint index : SV_OutputControlPointID)
{
	HullOut ret;

	ret.mWorldPosition = verticeData[ index ].mWorldPosition;
	ret.mWorldNormal = verticeData[ index ].mWorldNormal;
	ret.mTexcoord = verticeData[ index ].mTexcoord;

	return ret;
}

#endif