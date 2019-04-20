
# glm
SET( GLM_QUIET ON )
SET( GLM_TEST_ENABLE OFF )
SET( BUILD_SHARED_LIBS OFF )
SET( BUILD_STATIC_LIBS OFF )

# boost
SET( BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_56_0" PARENT_SCOPE )
SET( BOOST_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_56_0/boost" PARENT_SCOPE )

# assimp
SET( ASSIMP_BUILD_ASSIMP_TOOLS OFF )
SET( ASSIMP_BUILD_TESTS OFF )
SET( BUILD_SHARED_LIBS OFF )

# freeimage
# need manual cmake...
add_definitions(-D_CRT_SECURE_NO_WARNINGS -DLIBRAW_NODLL -DFREEIMAGE_LIB -DOPJ_STATIC )

add_subdirectory( glm )
add_subdirectory( dlmalloc )
add_subdirectory( assimp-3.3.1 )
add_subdirectory( FreeImage )

# add assimp include dirs
get_target_property( ASSIMP_INCLUDE assimp INCLUDE_DIRECTORIES )
target_include_directories( assimp PUBLIC ${ASSIMP_INCLUDE})
