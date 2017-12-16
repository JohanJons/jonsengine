#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"

float CalculateTessellationfactor(float3 patchViewPos)
{

}

PatchTess PatchHS(InputPatch<VertexOut, 3> inputVertices)
{
	PatchTess patch;

	// Average tess factors along edges, and pick an edge tess factor for 
	// the interior tessellation.  It is important to do the tess factor
	// calculation based on the edge properties so that edges shared by 
	// more than one triangle will have the same tessellation factor.  
	// Otherwise, gaps can appear.
	patch.mEdgeTess[0] = 0.5f*(inputVertices[1].mTessFactor + inputVertices[2].mTessFactor);
	patch.mEdgeTess[1] = 0.5f*(inputVertices[2].mTessFactor + inputVertices[0].mTessFactor);
	patch.mEdgeTess[2] = 0.5f*(inputVertices[0].mTessFactor + inputVertices[1].mTessFactor);
	patch.mInsideTess = patch.mEdgeTess[0];

	return patch;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOut hull_main(InputPatch<VertexOut, 3> verticeData, uint index : SV_OutputControlPointID)
{
	HullOut ret;

	ret.mWorldPosition = verticeData[index].mWorldPosition;

	return ret;
}

#endif