#pragma once

#include "include/Input/InputBackend.h"

#include "include/Input/KeyCallback.h"
#include "include/Input/MouseCallback.h"

namespace JonsEngine
{
    class Logger;

    /* GLFWInputBackend definition */
    /* TODO: Add support for multiple simulatinous GLFW backends.. but will add overhead */
    class GLFWInputBackend : public InputBackend
    {
    public:
        GLFWInputBackend(const MouseButtonCallback& onMouseButton, const MouseMotionCallback& onMouseMotion, const KeyCallback& onKey);
        ~GLFWInputBackend();

        InputBackendType GetInputBackendType() const;


    private:
        static void glfwOnMouseButton(int button, int action);
        static void glfwOnMouseMotion(int posX, int posY );
        static void glfwOnKey(int key, int action);

        const MouseButtonCallback& mOnMouseButton;
        const MouseMotionCallback& mOnMouseMotion;
        const KeyCallback& mOnKey;

        Logger& mLogger;
    };

    
    /* GLFWInputBackend inlines */
    inline InputBackend::InputBackendType GLFWInputBackend::GetInputBackendType() const       { return GLFW; }
}