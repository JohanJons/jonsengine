#ifndef TERRAIN_HULL_HLSL
#define TERRAIN_HULL_HLSL

#include "TerrainCommon.hlsl"

PatchTess PatchHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	// Average tess factors along edges, and pick an edge tess factor for 
	// the interior tessellation.  It is important to do the tess factor
	// calculation based on the edge properties so that edges shared by 
	// more than one triangle will have the same tessellation factor.  
	// Otherwise, gaps can appear.
	pt.EdgeTess[0] = 0.5f*(patch[1].TessFactor + patch[2].TessFactor);
	pt.EdgeTess[1] = 0.5f*(patch[2].TessFactor + patch[0].TessFactor);
	pt.EdgeTess[2] = 0.5f*(patch[0].TessFactor + patch[1].TessFactor);
	pt.InsideTess = pt.EdgeTess[0];

	return pt;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOut hull_main(InputPatch<VertexOut, 3> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.PosW = p[i].PosW;
	hout.NormalW = p[i].NormalW;
	hout.TangentW = p[i].TangentW;
	hout.Tex = p[i].Tex;

	return hout;
}

#endif