target_sources( JonsEngine
    PRIVATE
        AABB.cpp
        AABB.h
        Frustum.cpp
        Frustum.h
        Intersection.cpp
        Intersection.h
        Math.cpp
        Math.h
        Plane.cpp
        Plane.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
