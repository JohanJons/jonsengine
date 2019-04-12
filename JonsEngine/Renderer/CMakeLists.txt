target_sources( ${ENGINE_NAME}
    PUBLIC
        RenderDefs.cpp
        RenderDefs.h
        RenderSettings.h
        Shapes.cpp
        Shapes.h
)

add_subdirectory( DirectX11 )

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
