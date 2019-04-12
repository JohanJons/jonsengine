target_sources( ${ENGINE_NAME}
    PUBLIC
        RenderQueue.cpp
        RenderQueue.h
        RenderQueueTypes.cpp
        RenderQueueTypes.h
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
