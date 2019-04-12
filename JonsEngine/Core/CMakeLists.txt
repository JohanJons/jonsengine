SET ( SUBDIR_NAMES
	Containers
	Logging
	Math
	Memory
	Platform
	Utils
)

target_sources( ${ENGINE_NAME}
    PUBLIC
        DebugOptions.h
        Engine.h
        Engine.cpp
        EngineSettings.h
        Types.h
)

foreach( SUBDIR ${SUBDIR_NAMES})
	SET( SOURCES_GROUP_FILES "" )
	add_subdirectory( ${SUBDIR} )
endforeach()

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
