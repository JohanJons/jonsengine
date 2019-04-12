target_sources( JonsEngine
    PRIVATE
        HeapAligned.h
        HeapAllocator.cpp
        HeapAllocator.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
