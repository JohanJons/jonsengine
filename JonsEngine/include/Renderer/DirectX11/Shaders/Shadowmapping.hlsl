#ifndef SHADOWMAPPING_HLSL
#define SHADOWMAPPING_HLSL

static const float gNormalOffset = 5.5;


/**
 * Negates most visible shadow acne with very few instructions
 * TODO: "ScaleByDepth" also might help but expensive?
 */
float3 ShadowAcneNormalOffset(in float3 normal, in float normalLightAngle, in float shadowmapSize)
{
    const float texelSize = 2.0 / shadowmapSize;
    const float normalOffsetScale = saturate(1.0 - normalLightAngle);

    return normal * gNormalOffset * normalOffsetScale * texelSize;
}

#endif