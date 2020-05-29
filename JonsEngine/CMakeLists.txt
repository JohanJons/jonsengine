SET ( SUBDIR_NAMES
	Core
	Renderer
	RenderQueue
	Resources
	Scene
	Shaders
	Window
)

add_library( JonsEngine STATIC "" )

foreach( SUBDIR ${SUBDIR_NAMES})
	add_subdirectory( ${SUBDIR} )
endforeach()

# Build filters for VS
get_target_property( SOURCES JonsEngine SOURCES )
foreach( SOURCE ${SOURCES})
	get_filename_component(FULL_SOURCE_DIR "${SOURCE}" DIRECTORY)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" REL_SOURCE_DIR ${FULL_SOURCE_DIR} )
	string(REPLACE "/" "\\" REL_SOURCE_DIR_NO_SLASH "${REL_SOURCE_DIR}")
	source_group("${REL_SOURCE_DIR_NO_SLASH}" FILES "${SOURCE}")
endforeach()

target_sources( JonsEngine
    INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/JonsEngine.natvis
)

target_compile_definitions( JonsEngine PUBLIC BOOST_ALL_NO_LIB )
target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR} )

find_package( Boost 1.70 REQUIRED serialization filesystem )
target_link_libraries( JonsEngine JonsShaders glm dlmalloc Boost::serialization Boost::filesystem d3d11 Comctl32 winmm toml11 )
