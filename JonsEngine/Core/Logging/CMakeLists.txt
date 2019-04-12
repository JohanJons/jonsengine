target_sources( JonsEngine
    PRIVATE
        Logger.cpp
        Logger.h
        LogManager.cpp
        LogManager.h
        OutputStream.cpp
        OutputStream.h
        StreamBuf.cpp
        StreamBuf.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
