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

	//output.mPosition.x = ((VertexID << 1) & 2) * 2.0f - 1.0f;
	//output.mPosition.y = (VertexID == 0) * -4.0f + 1.0f;
	output.mPosition.z = 0.0;
	output.mPosition.w = 1.0;

	/*if (VertexID == 0) {
		output.mPosition.x = -1.0;
		output.mPosition.y = -3.0;
		output.mTexcoord = float2(0.0, 4.0);
	}
	if (VertexID == 1) {
		output.mPosition.x = 3.0;
		output.mPosition.y = 1.0;
		output.mTexcoord = float2(4.0, 0.0);
	}
	if (VertexID == 2) {
		output.mPosition.x = -1.0;
		output.mPosition.y = 1.0;
		output.mTexcoord = float2(0.0, 0.0);
	}*/

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

	// x  = ((VertexID << 1) & 2) * 2.0f,
	// y = (VertexID == 0) * -4.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	// 0 = -1, -3
	// 1 = 3, 1
	// 2 = -1, 1
	//output.mTexcoord = float2((VertexID << 1) & 2, VertexID & 2);
	//output.mPosition = float4(output.mTexcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	//output.mTexcoord = float2((VertexID << 1) & 2, VertexID & 2);
	//output.mPosition = float4(output.mTexcoord * float2(2, -2) + float2(-1, 1), 0, 1);
	//output.mTexcoord.x = (float)(VertexID / 2) * 2.0;
	//output.mTexcoord.y = 1.0 - (float)(VertexID % 2) * 2.0;

	return output;
}

#endif