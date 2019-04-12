target_sources( JonsEngine
    PRIVATE
        RenderDefs.cpp
        RenderDefs.h
        RenderSettings.h
        Shapes.cpp
        Shapes.h
)

add_subdirectory( DirectX11 )

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
