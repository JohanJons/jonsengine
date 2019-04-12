target_sources( ${ENGINE_NAME}
    PUBLIC
        HeapAligned.h
        HeapAllocator.cpp
        HeapAllocator.h
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
