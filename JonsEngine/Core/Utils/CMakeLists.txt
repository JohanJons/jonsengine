target_sources( JonsEngine
    PRIVATE
        Clock.hpp
        Error.h
        IDGenerator.hpp
        NonCopyable.h
        Time.h
        Timer.cpp
        Timer.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
