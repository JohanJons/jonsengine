target_sources( JonsEngine
    PRIVATE
        RenderQueue.cpp
        RenderQueue.h
        RenderQueueTypes.cpp
        RenderQueueTypes.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
