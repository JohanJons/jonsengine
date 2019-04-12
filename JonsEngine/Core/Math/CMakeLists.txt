target_sources( ${ENGINE_NAME}
    PUBLIC
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

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
