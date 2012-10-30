#include "include/Input/GLFW/GLFWInputBackend.h"

#include "include/Core/Logging/Logger.h"

#include "GL/glfw.h"


namespace JonsEngine
{
    static GLFWInputBackend* gGLFWInputBackendInstance = NULL;

    GLFWInputBackend::GLFWInputBackend(const MouseButtonCallback& onMouseButton, const MouseMotionCallback& onMouseMotion, const KeyCallback& onKey) : mLogger(Globals::GetInputLogger()),
                                                                                                                         mOnMouseButton(onMouseButton), mOnMouseMotion(onMouseMotion), mOnKey(onKey)
    {
        gGLFWInputBackendInstance = this;

        GLenum glfwErr = glfwInit();
        if (glfwErr != GL_TRUE)
            JONS_LOG_ERROR(mLogger, "GLFWInputBackend::GLFWInputBackend(): Unable to initialize InputManager!")

        glfwSetMouseButtonCallback(glfwOnMouseButton);
        glfwSetMousePosCallback(glfwOnMouseMotion);
        glfwSetCharCallback(glfwOnKey);
    }
        
    GLFWInputBackend::~GLFWInputBackend()
    {
        glfwSetCharCallback(NULL);
        glfwSetMouseButtonCallback(NULL);
        glfwSetMousePosCallback(NULL);

        gGLFWInputBackendInstance = NULL;
    }

    void GLFWInputBackend::glfwOnMouseButton(int button, int action)
    {
        MouseButtonEvent ev(button == GLFW_MOUSE_BUTTON_LEFT ? MouseButtonEvent::MOUSEBUTTON_LEFT : button == GLFW_MOUSE_BUTTON_MIDDLE ?
                                MouseButtonEvent::MOUSEBUTTON_MIDDLE : MouseButtonEvent::MOUSEBUTTON_RIGHT,
                                action == GLFW_PRESS ? MouseButtonEvent::PRESSED : MouseButtonEvent::RELEASED);

        gGLFWInputBackendInstance->mOnMouseButton(ev);
    }
    
    void GLFWInputBackend::glfwOnMouseMotion(int posX, int posY )
    {
        MouseMotionEvent ev(posX, posY);

        gGLFWInputBackendInstance->mOnMouseMotion(ev);
    }
   
    void GLFWInputBackend::glfwOnKey(int key, int action)
    {
        KeyEvent ev(key, action == GLFW_PRESS ? KeyEvent::PRESSED : KeyEvent::RELEASED);

        gGLFWInputBackendInstance->mOnKey(ev);
    }

}