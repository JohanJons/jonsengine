target_sources( ${ENGINE_NAME}
    PUBLIC
        Logger.cpp
        Logger.h
        LogManager.cpp
        LogManager.h
        OutputStream.cpp
        OutputStream.h
        StreamBuf.cpp
        StreamBuf.h
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
