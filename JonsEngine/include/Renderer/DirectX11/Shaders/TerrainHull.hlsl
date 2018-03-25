#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"

float CalculateTessellationfactor(float3 patchViewPos)
{

}

PatchTess PatchHS(InputPatch<VertexOut, 4> inputVertices)
{
	PatchTess patch;

	patch.mEdgeTess[ 0 ] = 16;
	patch.mEdgeTess[ 1 ] = 16;
	patch.mEdgeTess[ 2 ] = 16;
	patch.mEdgeTess[ 3 ] = 16;
	
	patch.mInsideTess[ 0 ] = 16;
	patch.mInsideTess[ 1 ] = 16;

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

	ret.mWorldPosition = verticeData[index].mWorldPosition;

	return ret;
}

#endif