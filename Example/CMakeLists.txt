set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup" )
add_executable( JonsExample "" )

target_sources( JonsExample
    PRIVATE
        Game.cpp
        Game.h
        main.cpp
        SceneActors.cpp
        SceneActors.h
        SceneTessellation.cpp
        SceneTessellation.h
        Sun.cpp
        Sun.h
)

get_target_property( SOURCES JonsExample SOURCES )
source_group("" FILES ${SOURCES} )

SET( OutputAndRunDir "${PROJECT_SOURCE_DIR}/bin" )
target_link_libraries( JonsExample JonsEngine )
set_target_properties( JonsExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OutputAndRunDir} VS_DEBUGGER_WORKING_DIRECTORY ${OutputAndRunDir} )
