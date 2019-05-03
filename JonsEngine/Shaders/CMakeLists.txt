list( APPEND LOCAL_VERTEX_SHADERS 
  FullscreenTriangle.hlsl
  FullscreenTriangleTexcoord.hlsl
  GBufferVertexStatic.hlsl
  GBufferVertexAnimated.hlsl
  SkyboxVertex.hlsl
  TerrainVertex.hlsl
  TransformAnimatedVertex.hlsl
  TransformStaticInstancedVertex.hlsl
  TransformStaticVertex.hlsl
)

list( APPEND LOCAL_PIXEL_SHADERS 
  AmbientPixel.hlsl
  AvgLuminance.hlsl
  BoxBlurPixel.hlsl
  DirectionalLightPCF2X2Pixel.hlsl
  DirectionalLightPCF3X3Pixel.hlsl
  DirectionalLightPCF5X5Pixel.hlsl
  DirectionalLightPCF7X7Pixel.hlsl
  FXAAPixel.hlsl
  PointLightPixel.hlsl
  SimpleColorPixel.hlsl
  SimpleTexturePixel.hlsl
  SkyboxPixel.hlsl
  SSAOPixel.hlsl
  TerrainPixel.hlsl
  TerrainPixelDebug.hlsl
  Tonemapping.hlsl
)

list( APPEND LOCAL_COMPUTE_SHADERS 
  DepthReadback.hlsl
  SDSMInitialCompute.hlsl
  SDSMFinalCompute.hlsl
)

list( APPEND LOCAL_DOMAIN_SHADERS 
  TerrainDomain.hlsl
)

list( APPEND LOCAL_HULL_SHADERS 
  TerrainHull.hlsl
)

list( APPEND LOCAL_NON_COMPILE_SHADERS 
  Common.hlsl
  Constants.h
  DirectionalLightPixel.hlsl
  FXAA.hlsl
  GBufferVertex.hlsl
  OptimizedPCF.hlsl
  Shadowmapping.hlsl
  TerrainCommon.hlsl
)

list( TRANSFORM LOCAL_VERTEX_SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list( TRANSFORM LOCAL_PIXEL_SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list( TRANSFORM LOCAL_COMPUTE_SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list( TRANSFORM LOCAL_DOMAIN_SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list( TRANSFORM LOCAL_HULL_SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list( TRANSFORM LOCAL_NON_COMPILE_SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

set ( VERTEX_SHADERS ${LOCAL_VERTEX_SHADERS} PARENT_SCOPE )
set ( PIXEL_SHADERS ${LOCAL_PIXEL_SHADERS} PARENT_SCOPE )
set ( COMPUTE_SHADERS ${LOCAL_COMPUTE_SHADERS} PARENT_SCOPE )
set ( DOMAIN_SHADERS ${LOCAL_DOMAIN_SHADERS} PARENT_SCOPE )
set ( HULL_SHADERS ${LOCAL_HULL_SHADERS} PARENT_SCOPE )
set ( NON_COMPILE_SHADERS ${LOCAL_NON_COMPILE_SHADERS} PARENT_SCOPE )

#add_library( JonsShaders STATIC ${VERTEX_SHADERS} ${NON_COMPILE_SHADERS} )
