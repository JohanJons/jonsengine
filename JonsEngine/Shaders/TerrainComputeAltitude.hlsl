#ifndef TERRAIN_COMPUTE_ALTITUDE_HLSL
#define TERRAIN_COMPUTE_ALTITUDE_HLSL

#include "Constants.hlsl"
#include "Common.hlsl"
#include "TerrainCommon.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<float> gAverageHeight : register( UAV_REGISTER_0 );

cbuffer KernelSize : register( CBUFFER_REGISTER_EXTRA )
{
    int gKernelSize;
}

[numthreads(TERRAIN_NORMAL_THREADS_AXIS, TERRAIN_NORMAL_THREADS_AXIS, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	const int2 texCoord = dispatchTID.xy;

    const int kernelSize = 3;
    const int xHalfSize = kernelSize / 2;
    const int yHalfSize = kernelSize / 2;

    float totalHeight = 0.0f;
    for ( int offsetX = -xHalfSize; offsetX <= xHalfSize; ++offsetX )
    {
        for ( int offsetY = -yHalfSize; offsetY <= yHalfSize; ++offsetY )
        {
            const int2 offsetTexcoord = texCoord + int2( offsetX, offsetY );
            totalHeight += gHeightmap[ offsetTexcoord ].r;
        }
    }

    const int numSamples = kernelSize * kernelSize;
    const float averageHeight = totalHeight / numSamples;

    gAverageHeight[ texCoord ] = averageHeight;
}

#endif