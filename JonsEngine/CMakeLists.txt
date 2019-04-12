SET ( SUBDIR_NAMES
	Core
	Renderer
	RenderQueue
	Resources
	Scene
	Window
)

set( ENGINE_NAME "JonsEngine" )

add_library( ${ENGINE_NAME} STATIC "" )

foreach( SUBDIR ${SUBDIR_NAMES})
	SET( SOURCES_GROUP_FILES "" )
	add_subdirectory( ${SUBDIR} )
	#source_group( ${SUBDIR} REGULAR_EXPRESSION "${SUBDIR}")
endforeach()

# Build filters for VS
get_target_property( SOURCES ${ENGINE_NAME} SOURCES )
foreach( SOURCE ${SOURCES})
	get_filename_component(FULL_SOURCE_DIR "${SOURCE}" DIRECTORY)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" REL_SOURCE_DIR ${FULL_SOURCE_DIR} )
	string(REPLACE "/" "\\" REL_SOURCE_DIR_NO_SLASH "${REL_SOURCE_DIR}")
	source_group("${REL_SOURCE_DIR_NO_SLASH}" FILES "${SOURCE}")
endforeach()
