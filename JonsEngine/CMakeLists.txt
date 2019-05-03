SET ( SUBDIR_NAMES
	Core
	Renderer
	RenderQueue
	Resources
	Scene
	Window
)

# Shaders needs special treatment
add_subdirectory( Shaders )

set_source_files_properties( ${VERTEX_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT vs_main VS_SHADER_TYPE Vertex )
set_source_files_properties( ${PIXEL_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT ps_main VS_SHADER_TYPE Pixel )
set_source_files_properties( ${COMPUTE_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT cs_main VS_SHADER_TYPE Compute )
set_source_files_properties( ${HULL_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT hs_main VS_SHADER_TYPE Hull )
set_source_files_properties( ${DOMAIN_SHADERS} PROPERTIES VS_SHADER_ENTRYPOINT ds_main VS_SHADER_TYPE Domain )
set_source_files_properties( ${NON_COMPILE_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE )

list( APPEND SHADER_SOURCES ${VERTEX_SHADERS} ${PIXEL_SHADERS} ${COMPUTE_SHADERS} ${HULL_SHADERS} ${NON_COMPILE_SHADERS} )
foreach( SHADER ${SHADER_SOURCES})
	get_filename_component(SHADER_FILENAME "${SHADER}" NAME_WE)
	message( ${SHADER_FILENAME} )
	set_source_files_properties( ${SHADER} PROPERTIES VS_SHADER_OBJECT_FILE_NAME "Shaders/${SHADER_FILENAME}.h" VS_SHADER_MODEL 5.0 )
endforeach()

add_library( JonsEngine STATIC ${VERTEX_SHADERS} ${NON_COMPILE_SHADERS} )

foreach( SUBDIR ${SUBDIR_NAMES})
	add_subdirectory( ${SUBDIR} )
endforeach()

# Build filters for VS
get_target_property( SOURCES JonsEngine SOURCES )
foreach( SOURCE ${SOURCES})
	#message( ${SOURCE} )
	get_filename_component(FULL_SOURCE_DIR "${SOURCE}" DIRECTORY)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" REL_SOURCE_DIR ${FULL_SOURCE_DIR} )
	string(REPLACE "/" "\\" REL_SOURCE_DIR_NO_SLASH "${REL_SOURCE_DIR}")
	source_group("${REL_SOURCE_DIR_NO_SLASH}" FILES "${SOURCE}")
endforeach()

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Shaders )

find_package( Boost 1.70 REQUIRED serialization )
target_link_libraries( JonsEngine JonsShaders glm dlmalloc Boost::serialization )