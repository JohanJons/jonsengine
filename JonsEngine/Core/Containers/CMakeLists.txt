target_sources( JonsEngine
    PRIVATE
        DataPool.hpp
        FixedGridQuadTree.hpp
        IDMap.hpp
        IDMapBase.hpp
        IDMapPointer.hpp
        IDMapTree.hpp
        RangedIterator.hpp
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
