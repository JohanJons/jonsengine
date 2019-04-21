SET ( SUBDIR_NAMES
	Containers
	Logging
	Math
	Memory
	Utils
)

target_sources( JonsEngine
    PRIVATE
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
