list( APPEND VERTEX_SHADERS 
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

list( APPEND PIXEL_SHADERS 
  AmbientPixel.hlsl
  AvgLuminance.hlsl
  BoxBlurPixel.hlsl
  DirectionalLightPCF2X2Pixel.hlsl
  DirectionalLightPCF3X3Pixel.hlsl
  DirectionalLightPCF5X5Pixel.hlsl
  DirectionalLightPCF7X7Pixel.hlsl
  FXAAPixel.hlsl
  GBufferPixel.hlsl
  PointLightPixel.hlsl
  SimpleColorPixel.hlsl
  SimpleTexturePixel.hlsl
  SkyboxPixel.hlsl
  SSAOPixel.hlsl
  TerrainPixel.hlsl
  TerrainPixelDebug.hlsl
  Tonemapping.hlsl
)

list( APPEND COMPUTE_SHADERS 
  DepthReadback.hlsl
  SDSMInitialCompute.hlsl
  SDSMFinalCompute.hlsl
)

list( APPEND DOMAIN_SHADERS 
  TerrainDomain.hlsl
)

list( APPEND HULL_SHADERS 
  TerrainHull.hlsl
)

list( APPEND NON_COMPILE_SHADERS 
  Common.hlsl
  Constants.hlsl
  DirectionalLightPixel.hlsl
  FXAA.hlsl
  GBufferVertex.hlsl
  OptimizedPCF.hlsl
  Shadowmapping.hlsl
  TerrainCommon.hlsl
)

add_library( JonsShaders OBJECT ${VERTEX_SHADERS} ${PIXEL_SHADERS} ${COMPUTE_SHADERS} ${DOMAIN_SHADERS} ${HULL_SHADERS} ${NON_COMPILE_SHADERS} )
set_target_properties( JonsShaders PROPERTIES LINKER_LANGUAGE CXX )
target_include_directories( JonsEngine PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" )

set_source_files_properties( ${VERTEX_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT vs_main VS_SHADER_TYPE Vertex )
set_source_files_properties( ${PIXEL_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT ps_main VS_SHADER_TYPE Pixel )
set_source_files_properties( ${COMPUTE_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT cs_main VS_SHADER_TYPE Compute )
set_source_files_properties( ${HULL_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT hs_main VS_SHADER_TYPE Hull )
set_source_files_properties( ${DOMAIN_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT ds_main VS_SHADER_TYPE Domain )
set_source_files_properties( ${NON_COMPILE_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE )

list( APPEND SHADER_SOURCES ${VERTEX_SHADERS} ${PIXEL_SHADERS} ${COMPUTE_SHADERS} ${HULL_SHADERS} ${DOMAIN_SHADERS} ${NON_COMPILE_SHADERS} )
foreach( SHADER ${SHADER_SOURCES})
	get_filename_component(SHADER_FILENAME "${SHADER}" NAME_WE)
	set_source_files_properties( ${SHADER} PROPERTIES VS_SHADER_OUTPUT_HEADER_FILE "Compiled/${SHADER_FILENAME}.h" VS_SHADER_VARIABLE_NAME "g${SHADER_FILENAME}" VS_SHADER_MODEL 5.0 )
endforeach()
