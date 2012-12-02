#include "include/Input/GLFW/GLFWInputBackend.h"

#include "include/Core/Logging/Logger.h"

#include "GL/glfw.h"
#include <exception>

namespace JonsEngine
{
    static GLFWInputBackend* gGLFWInputBackendInstance = NULL;

    GLFWInputBackend::GLFWInputBackend() : mOnMouseButton(NULL), mOnMouseMotion(NULL), mOnKey(NULL), mLogger(Logger::GetInputLogger())
    {
        GLenum glfwErr = glfwInit();
        if (glfwErr != GL_TRUE)
        {
            JONS_LOG_ERROR(mLogger, "GLFWInputBackend::GLFWInputBackend(): Unable to initialize InputManager!")
            throw std::exception("GLFWInputBackend::GLFWInputBackend(): Unable to initialize InputManager!");
        }

        gGLFWInputBackendInstance = this;
    }
        
    GLFWInputBackend::~GLFWInputBackend()
    {
        glfwSetCharCallback(NULL);
        glfwSetMouseButtonCallback(NULL);
        glfwSetMousePosCallback(NULL);

        gGLFWInputBackendInstance = NULL;
    }

    void GLFWInputBackend::SetMouseButtonCallback(const MouseButtonCallback& onMouseButton)
    { 
        mOnMouseButton = onMouseButton;
        glfwSetMouseButtonCallback(glfwOnMouseButton);
    }

    void GLFWInputBackend::SetMouseMotionCallback(const MouseMotionCallback& onMouseMotion)
    { 
        mOnMouseMotion = onMouseMotion;
        glfwSetMousePosCallback(glfwOnMouseMotion);
    }

    void GLFWInputBackend::SetKeyCallback(const KeyCallback& onKey)
    { 
        mOnKey = onKey;
        glfwSetCharCallback(glfwOnKey);
    }

    bool GLFWInputBackend::IsMouseButtonCallbackSet() const                               { return mOnMouseButton != NULL;    }
    bool GLFWInputBackend::IsMouseMotionCallbackSet() const                               { return mOnMouseMotion != NULL;    }
    bool GLFWInputBackend::IsKeyCallbackSet() const                                       { return mOnKey != NULL;            }

    void GLFWInputBackend::glfwOnMouseButton(int button, int action)
    {
        if (gGLFWInputBackendInstance != NULL && gGLFWInputBackendInstance->IsMouseButtonCallbackSet())
        {
            MouseButtonEvent ev(button == GLFW_MOUSE_BUTTON_LEFT ? MouseButtonEvent::MOUSEBUTTON_LEFT : button == GLFW_MOUSE_BUTTON_MIDDLE ?
                                    MouseButtonEvent::MOUSEBUTTON_MIDDLE : MouseButtonEvent::MOUSEBUTTON_RIGHT,
                                    action == GLFW_PRESS ? MouseButtonEvent::PRESSED : MouseButtonEvent::RELEASED);

            gGLFWInputBackendInstance->mOnMouseButton(ev);
        }
    }
    
    void GLFWInputBackend::glfwOnMouseMotion(int posX, int posY )
    {
        if (gGLFWInputBackendInstance != NULL && gGLFWInputBackendInstance->IsMouseMotionCallbackSet())
        {
            MouseMotionEvent ev(posX, posY);

            gGLFWInputBackendInstance->mOnMouseMotion(ev);
        }
    }
   
    void GLFWInputBackend::glfwOnKey(int key, int action)
    {
        if (gGLFWInputBackendInstance != NULL && gGLFWInputBackendInstance->IsKeyCallbackSet())
        {
            KeyEvent ev(key, action == GLFW_PRESS ? KeyEvent::PRESSED : KeyEvent::RELEASED);

            gGLFWInputBackendInstance->mOnKey(ev);
        }
    }

}