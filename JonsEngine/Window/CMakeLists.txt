target_sources( JonsEngine
    PRIVATE
        KeyCallback.h
        KeyDefinition.h
        MouseCallback.h
        WindowManager.cpp
        WindowManager.h
        WindowSettings.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
