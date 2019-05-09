cmake_minimum_required (VERSION 3.14)
project (JonsEngine)

add_subdirectory( Thirdparty )
add_subdirectory( JonsEngine )
add_subdirectory( Example )
add_subdirectory( Tools )

set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT JonsExample )
