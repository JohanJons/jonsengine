
# glm
SET( GLM_QUIET ON CACHE BOOL "" FORCE )
SET( GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE )
SET( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
SET( BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE )

# boost
SET( BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE )
SET( BOOST_INCLUDEDIR "${BOOST_ROOT}/boost" PARENT_SCOPE )
SET( BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib" PARENT_SCOPE )

# assimp
SET( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE )
SET( ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE )
SET( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )

add_subdirectory( glm )
add_subdirectory( dlmalloc )
add_subdirectory( assimp-3.3.1 ${CMAKE_BINARY_DIR}/assimp )
add_subdirectory( FreeImage )

# add assimp include dirs
get_target_property( ASSIMP_INCLUDE assimp INCLUDE_DIRECTORIES )
target_include_directories( assimp PUBLIC ${ASSIMP_INCLUDE})
set_target_properties( assimp PROPERTIES OUTPUT_NAME "Assimp")
