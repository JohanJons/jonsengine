target_sources( ${ENGINE_NAME}
    PUBLIC
        KeyCallback.h
        KeyDefinition.h
        MouseCallback.h
        WindowManager.cpp
        WindowManager.h
        WindowSettings.h
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
