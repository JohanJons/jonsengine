#ifndef TERRAIN_COMPUTE_HLSL
#define TERRAIN_COMPUTE_HLSL

#include "Constants.hlsl"

Texture2D gHeightmap : register( TEXTURE_REGISTER_EXTRA );
RWTexture2D<unorm float> gCoplanarityTexture : register(UAV_REGISTER);

groupshared float gDepthSamples[ TERRAIN_COMPUTE_PATCH_EXTENT * TERRAIN_COMPUTE_PATCH_EXTENT ];
groupshared float4 gPlane;
groupshared float3 gCorners[ 2 ][ 2 ];
groupshared float3 gRawNormals[ 2 ][ 2 ];

float3 ComputeCornerNormal( float3 a, float3 b, float3 c )
{
    return normalize( cross( b - a, c - a ) );
}

[numthreads(TERRAIN_COMPUTE_PATCH_EXTENT, TERRAIN_COMPUTE_PATCH_EXTENT, 1)]
void cs_main(uint3 groupID : SV_GroupID, uint3 dispatchTID : SV_DispatchThreadID, uint3 groupTID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
    uint lastThreadIndex = TERRAIN_COMPUTE_PATCH_EXTENT - 1;
    bool isCorner00 = ( groupTID.x == 0 && groupTID.y == 0 );
    bool isCorner10 = ( groupTID.x == lastThreadIndex && groupTID.y == 0 );
    bool isCorner01 = ( groupTID.x == 0 && groupTID.y == lastThreadIndex );
    bool isCorner11 = ( groupTID.x == lastThreadIndex && groupTID.y == lastThreadIndex );
    bool isCornerThread = isCorner00 || isCorner10 || isCorner01 || isCorner11;

    // step 1: corner threads
    if ( isCornerThread )
    {
        gDepthSamples[ groupIndex ] = gHeightmap.Load( uint3( dispatchTID.xy, 0 ) ).r;

        uint cornerX = groupTID.x / lastThreadIndex;
        uint cornerZ = groupTID.z / lastThreadIndex;
        float bias = 64.0f;
        gCorners[ cornerX ][ cornerZ ] = float3( cornerX / bias, gDepthSamples[ groupIndex ], cornerZ / bias );
    }

    GroupMemoryBarrierWithGroupSync();

    // step 2: compute normals from corners and load the rest of the heightmap patch
    // offset by one if corner, since we already loaded those

    if ( isCorner00 )
        gRawNormals[ 0 ][ 0 ] = ComputeCornerNormal( gCorners[ 0 ][ 0 ], gCorners[ 0 ][ 1 ], gCorners[ 1 ][ 0 ] );
    else if ( isCorner10 )
        gRawNormals[ 1 ][ 0 ] = ComputeCornerNormal( gCorners[ 1 ][ 0 ], gCorners[ 0 ][ 0 ], gCorners[ 1 ][ 1 ] );
    else if ( isCorner01 )
        gRawNormals[ 0 ][ 1 ] = ComputeCornerNormal( gCorners[ 0 ][ 1 ], gCorners[ 1 ][ 1 ], gCorners[ 0 ][ 0 ] );
    else if ( isCorner10 )
        gRawNormals[ 1 ][ 1 ] = ComputeCornerNormal( gCorners[ 1 ][ 1 ], gCorners[ 1 ][ 0 ], gCorners[ 0 ][ 1 ] );
    else
    {
        gDepthSamples[ groupIndex ] = gHeightmap.Load( uint3( dispatchTID.xy, 0 ) ).r;
    }

    GroupMemoryBarrierWithGroupSync();
}

#endif