
# glm
SET( GLM_QUIET ON )
SET( BUILD_SHARED_LIBS OFF )
SET( BUILD_STATIC_LIBS OFF )

# boost
SET( BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_56_0" PARENT_SCOPE )
SET( BOOST_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_56_0/boost" PARENT_SCOPE )
#message( ${BOOST_ROOT} )
#message( ${BOOST_INCLUDEDIR} )
#find_package(Boost 1.56 REQUIRED)

add_subdirectory( glm )
add_subdirectory( dlmalloc )
