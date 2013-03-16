#pragma once

#include "include/Input/InputBackend.h"

namespace JonsEngine
{
    class Logger;

    /* GLFWInputBackend definition */
    /* TODO: Add support for multiple simulatinous GLFW backends.. but will add overhead */
    class GLFWInputBackend : public InputBackend
    {
    public:
        GLFWInputBackend();
        ~GLFWInputBackend();

        void SetMouseButtonCallback(const MouseButtonCallback& onMouseButton);
        void SetMouseMotionCallback(const MouseMotionCallback& onMouseMotion);
        void SetKeyCallback(const KeyCallback& onKey);

        bool IsMouseButtonCallbackSet() const;
        bool IsMouseMotionCallbackSet() const;
        bool IsKeyCallbackSet() const;

        void ShowMouseCursor(bool show);
        void SetMousePosition(uint32_t x, uint32_t y);

        InputBackendType GetInputBackendType() const;


    private:
        static void glfwOnMouseButton(int button, int action);
        static void glfwOnMouseMotion(int posX, int posY );
        static void glfwOnKey(int key, int action);
        static Key ParseGLFWKey(int key);

        MouseButtonCallback mOnMouseButton;
        MouseMotionCallback mOnMouseMotion;
        KeyCallback mOnKey;

        Logger& mLogger;
    };

    
    /* GLFWInputBackend inlines */
    inline InputBackend::InputBackendType GLFWInputBackend::GetInputBackendType() const       { return GLFW; }
}