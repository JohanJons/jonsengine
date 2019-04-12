target_sources( ${ENGINE_NAME}
    PUBLIC
        DataPool.hpp
        FixedGridQuadTree.hpp
        IDMap.hpp
        IDMapBase.hpp
        IDMapPointer.hpp
        IDMapTree.hpp
        RangedIterator.hpp
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
