#ifndef OPTIMIZED_PCF_HLSL
#define OPTIMIZED_PCF_HLSL

//
// Thanks to the authors of "The Witness" for large parts of the shadercode
// NOTE: define PCF_KERNEL_SIZE prior to including this for different PCF kernels
//
#ifndef PCF_KERNEL_SIZE
    #define PCF_KERNEL_SIZE 2
#endif

float SampleOptimizedPCF(in Texture2D shadowmap)
{
	float2 shadowMapSize;
	float numSlices;
	ShadowMap.GetDimensions(shadowMapSize.x, shadowMapSize.y, numSlices);

	float lightDepth = shadowPos.z;
}

float SampleOptimizedPCF(in Texture2DArray shadowmapArr)
{
	float2 shadowMapSize;
	float numSlices;
	ShadowMap.GetDimensions(shadowMapSize.x, shadowMapSize.y, numSlices);

	float lightDepth = shadowPos.z;
}

#endif