add_library( dlmalloc STATIC "" )

target_sources( dlmalloc
    PRIVATE
        dlmalloc.c
        dlmalloc.h
)

get_target_property( SOURCES dlmalloc SOURCES )
source_group("" FILES ${SOURCES} )

target_include_directories( dlmalloc PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
