target_sources( ${ENGINE_NAME}
    PUBLIC
        Clock.hpp
        Error.h
        IDGenerator.hpp
        NonCopyable.h
        Time.h
        Timer.cpp
        Timer.h
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
