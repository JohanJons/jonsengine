#ifndef FULLSCREEN_TRIANGLE_TEXCCOORD_HLSL
#define FULLSCREEN_TRIANGLE_TEXCCOORD_HLSL

struct FullScreenTexcoordOutput
{
	float4 mPosition : SV_POSITION;
	float2 mTexcoord : TEXCOORD;
};

// Variation on Timothy Lottes FXAA vertex shader
FullScreenTexcoordOutput vs_main(uint VertexID: SV_VertexID)
{
	FullScreenTexcoordOutput output = (FullScreenTexcoordOutput)0;

	if (VertexID == 0) {
		output.mPosition.x = -1.0;
		output.mPosition.y = 1.0;
		output.mTexcoord = float2(0.0, 0.0);
	}
	if (VertexID == 1) {
		output.mPosition.x = -1.0;
		output.mPosition.y = -3.0;
		output.mTexcoord = float2(0.0, 2.0);
	}
	if (VertexID == 2) {
		output.mPosition.x = 3.0;
		output.mPosition.y = 1.0;
		output.mTexcoord = float2(2.0, 0.0);
	}

	output.mPosition.z = 0.0;
	output.mPosition.w = 1.0;

	return output;
}

#endif