#include "include/Window/GLFW/GLFWWindowManager.h"

#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <exception>
#include <thread>

namespace JonsEngine
{
    static GLFWWindowManager* gGLFWWindowInstance = nullptr;

    GLFWWindowManager::GLFWWindowManager(const EngineSettings& engineSettings, OnContextCreatedCallback contextCreatedCallback) : mOnContextCreated(contextCreatedCallback), 
                                                                                                                                  mLogger(Logger::GetWindowLogger()), mFrameLimit(engineSettings.mFrameLimit), mWindow(nullptr),
                                                                                                                                  mScreenWidth(engineSettings.mWindowWidth), mScreenHeight(engineSettings.mWindowHeight), mShowMouseCursor(false), mFullscreen(engineSettings.mFullscreen), mFOV(engineSettings.mFOV),
                                                                                                                                  mWindowTitle(engineSettings.mWindowTitle), mMSAA(engineSettings.mMSAA)
    {
        if (glfwInit() != GL_TRUE)
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::GLFWWindow(): Unable to initialize GLFW!");
            throw std::runtime_error("GLFWWindow::GLFWWindow(): Unable to initialize GLFW!");
        }
        
        // setup a forward-compatible context with openGL 4.2
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        SetupWindow();

        gGLFWWindowInstance = this;
    }
        
    GLFWWindowManager::~GLFWWindowManager()
    {
        glfwSetKeyCallback(mWindow, nullptr);
        glfwSetMouseButtonCallback(mWindow, nullptr);
        glfwSetCursorPosCallback(mWindow, nullptr);
        glfwTerminate();
        gGLFWWindowInstance = nullptr;
    }

        
    void GLFWWindowManager::StartFrame()
    {
        mLastFrameTime = mThisFrameTime;
        if (mFrameLimit)
            mStartFrameTime = glfwGetTime();

        glfwPollEvents();
    }
        
    void GLFWWindowManager::EndFrame()
    {
        if (mFrameLimit)
        {
            mThisFrameTime = glfwGetTime();

            const double endFrameTime = (mThisFrameTime - mStartFrameTime);
            const double frameTime = 1.0f / mFrameLimit;

            if (endFrameTime < frameTime)
            {
                const unsigned long sleepTimeMs = static_cast<unsigned long>((frameTime - endFrameTime) * 1000);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
            }
        }

        glfwSwapBuffers(mWindow);
    }


    void GLFWWindowManager::SetMouseButtonCallback()
    {
        mMouseButtonCallback = nullptr;
        glfwSetMouseButtonCallback(mWindow, nullptr);
    }

    void GLFWWindowManager::SetMouseButtonCallback(const MouseButtonCallback& callback)
    {
        mMouseButtonCallback = callback;
        glfwSetMouseButtonCallback(mWindow, glfwOnMouseButton);
    }

    void GLFWWindowManager::SetMouseMotionCallback()
    {
        mMouseMotionCallback = nullptr;
        glfwSetCursorPosCallback(mWindow, nullptr);
    }
        
    void GLFWWindowManager::SetMouseMotionCallback(const MouseMotionCallback& callback)
    {
        mMouseMotionCallback = callback;
        glfwSetCursorPosCallback(mWindow, glfwOnMouseMotion);
    }
	
	void GLFWWindowManager::SetMousePositionCallback()
	{
		mMousePositionCallback = nullptr;
	}
        
	void GLFWWindowManager::SetMousePositionCallback(const MousePositionCallback& callback)
	{
		mMousePositionCallback = callback;
	}

	
    void GLFWWindowManager::SetKeyCallback()
    {
        mKeyCallback = nullptr;
        glfwSetKeyCallback(mWindow, nullptr);
    }
        
    void GLFWWindowManager::SetKeyCallback(const KeyCallback& callback)
    {
        mKeyCallback = callback;
        glfwSetKeyCallback(mWindow, glfwOnKey);
    }

        
    void GLFWWindowManager::ShowMouseCursor(bool show)
    {
        mShowMouseCursor = show;
        glfwSetInputMode(mWindow, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
        
    void GLFWWindowManager::SetMousePosition(uint32_t x, uint32_t y)
    {
        glfwSetCursorPos(mWindow, x, y);
    }

    /*
     * Dispatches queued input events to all registered callbacks.
     * Is called by the Engine once per frame.
     */
    void GLFWWindowManager::Poll()
    {
        // Dispatch waiting events for each event type to registered callbacks
        for(MouseButtonEvent& ev : mMouseButtonEvents)
            mMouseButtonCallback(ev);

        for(MouseMotionEvent& ev : mMouseMotionEvents)
            mMouseMotionCallback(ev);

        for(KeyEvent& ev : mKeyEvents)
            mKeyCallback(ev);
			
		if (mMousePositionCallback)
		{
			
		}

        ClearAllEvents();
    }

    void GLFWWindowManager::ClearAllEvents()
    {
        mMouseButtonEvents.clear();
        mMouseMotionEvents.clear();
        mKeyEvents.clear();
    }

    
    void GLFWWindowManager::SetFrameLimit(const uint64_t frameLimit)
    {
        mFrameLimit = frameLimit;
    }
        
    uint64_t GLFWWindowManager::GetCurrentFPS() const
    {
        return static_cast<uint64_t>(1 / (mThisFrameTime - mLastFrameTime));
    }

        
    void GLFWWindowManager::SetFullscreen(bool fullscreen)
    {
        if (mFullscreen != fullscreen)
        {
            mFullscreen = fullscreen;
            SetupWindow();
        }
    }
        
    void GLFWWindowManager::SetScreenResolution(const uint32_t width, const uint32_t height)
    {
        if (mFullscreen)
            return;

        if (mScreenWidth != width || mScreenHeight != height)
        {
            glfwSetWindowSize(mWindow, width, height);

            mScreenWidth  = width;
            mScreenHeight = height;

            SetupWindow();
        }
    }
        
    void GLFWWindowManager::SetWindowTitle(const std::string& windowTitle)
    {
        if (mWindowTitle.compare(windowTitle) != 0)
            glfwSetWindowTitle(mWindow, windowTitle.c_str());

        mWindowTitle = windowTitle;
    }

    void GLFWWindowManager::SetFOV(const float FOV)
    {
        mFOV = FOV;
    }

    void GLFWWindowManager::SetMSAA(const uint16_t msaa)
    {
        mMSAA = msaa;
        SetupWindow();
    }
        

    uint32_t GLFWWindowManager::GetScreenWidth() const
    {
        return mScreenWidth;
    }
        
    uint32_t GLFWWindowManager::GetScreenHeight() const
    {
        return mScreenHeight;
    }

        
    bool GLFWWindowManager::SetupWindow()
    {
        glfwWindowHint(GLFW_SAMPLES, mMSAA);
        auto newWindow = glfwCreateWindow(mScreenWidth, mScreenHeight, mWindowTitle.c_str(), mFullscreen ? glfwGetPrimaryMonitor() : nullptr, mWindow);

        if (!newWindow) 
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::SetupWindow(): Failed to create main window!")
            return false;
        }

        glfwMakeContextCurrent(newWindow);
        InitializeGLEW();

        if (mWindow) {
            glfwDestroyWindow(mWindow);
            mOnContextCreated();
        }
        mWindow = newWindow;

        // listen to input callbacks
        if (mMouseButtonCallback) glfwSetMouseButtonCallback(mWindow, glfwOnMouseButton);
        if (mMouseMotionCallback) glfwSetCursorPosCallback(mWindow, glfwOnMouseMotion);
        if (mKeyCallback) glfwSetKeyCallback(mWindow, glfwOnKey);

        glfwSetFramebufferSizeCallback(mWindow, glfwOnWindowChanged);

        ShowMouseCursor(mShowMouseCursor);
        glfwSetWindowPos(mWindow, 0, 0);
        // TODO: Move this into OpenGL somewhere
        glViewport(0, 0, (GLsizei) mScreenWidth, (GLsizei)mScreenHeight);

        return true;
    }

    void GLFWWindowManager::InitializeGLEW()
    {
        // 'glewExperimental = GL_TRUE' needed to initialize openGL core profile; see GLEW doc
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();

        if (glewErr != GLEW_OK || !GLEW_VERSION_3_3)
        {
            JONS_LOG_ERROR(mLogger, "GLFWWindow::GLFWWindow(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
            throw std::runtime_error("GLFWWindow::GLFWWindow(): Minimum OpenGL driver (OpenGL 3.3) not supported!");
        }

        glewErr = glGetError();
        if (glewErr == GL_INVALID_ENUM)
            JONS_LOG_INFO(mLogger, "GLFWWindow::GLFWWindow(): GL_INVALID_ENUM after glewInit, assumed due to glewExperimental = GL_TRUE");
    }
        
    void GLFWWindowManager::glfwOnWindowChanged(::GLFWwindow* window, int width, int height)
    {
        if (gGLFWWindowInstance == nullptr || window != gGLFWWindowInstance->mWindow)
            return;

        glViewport(0, 0, (GLsizei) width, (GLsizei)height);
    }


    void GLFWWindowManager::glfwOnMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        if (gGLFWWindowInstance == nullptr || !gGLFWWindowInstance->mMouseButtonCallback || window != gGLFWWindowInstance->mWindow)
            return;

        gGLFWWindowInstance->mMouseButtonEvents.push_back(MouseButtonEvent([&]()
                            {
                                switch (button)
                                {
                                    case GLFW_MOUSE_BUTTON_LEFT:    return MouseButtonEvent::BUTTON_LEFT;
                                    case GLFW_MOUSE_BUTTON_MIDDLE:  return MouseButtonEvent::BUTTON_MIDDLE;
                                    case GLFW_MOUSE_BUTTON_RIGHT:   return MouseButtonEvent::BUTTON_RIGHT;
                                    default:                        return MouseButtonEvent::BUTTON_NONE;
                                }
                            }(),
                            [&]()
                            {
                                switch (action)
                                {
                                    case GLFW_PRESS:    return MouseButtonEvent::STATE_PRESSED;
                                    case GLFW_RELEASE:  return MouseButtonEvent::STATE_RELEASED;
                                    default:            return MouseButtonEvent::STATE_UNKNOWN;
                                }
                            }(),
                            [&]()
                            {
                                switch (mods)
                                {
                                    case GLFW_MOD_SHIFT:    return MouseButtonEvent::MODIFIER_SHIFT;
                                    case GLFW_MOD_ALT:      return MouseButtonEvent::MODIFIER_ALT;
                                    case GLFW_MOD_CONTROL:  return MouseButtonEvent::MODIFIER_CONTROL;
                                    default:                return MouseButtonEvent::MODIFIER_NONE;
                                }
                            }()));
    }
    
    void GLFWWindowManager::glfwOnMouseMotion(GLFWwindow* window, double posX, double posY)
    {
        if (gGLFWWindowInstance == nullptr || !gGLFWWindowInstance->mMouseMotionCallback || window != gGLFWWindowInstance->mWindow)
            return;

        gGLFWWindowInstance->mMouseMotionEvents.push_back(MouseMotionEvent(posX, posY));
    }
   
    void GLFWWindowManager::glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (gGLFWWindowInstance == nullptr || !gGLFWWindowInstance->mKeyCallback || window != gGLFWWindowInstance->mWindow)
            return;

        gGLFWWindowInstance->mKeyEvents.push_back(KeyEvent(
                            [&]()
                            {
                                switch (key)
                                {
                                    case GLFW_KEY_UNKNOWN:      return UNKNOWN;
                                    case GLFW_KEY_SPACE:        return SPACE;
                                    case 33:                    return EXCLAMATION_MARK; 
                                    case 34:                    return QUOTE;
                                    case 35:                    return HASH;
                                    case 36:                    return DOLLAR;
                                    case 37:                    return PERCENT;
                                    case 38:                    return AMPERSAND;
                                    case 39:                    return APOSTROPHE;
                                    case 40:                    return LEFT_PARENTHESIS;
                                    case 41:                    return RIGHT_PARENTHESIS;
                                    case 42:                    return ASTERISK;
                                    case 43:                    return PLUS;
                                    case 44:                    return COMMA;
                                    case 45:                    return MINUS;
                                    case 46:                    return DOT;
                                    case 47:                    return RIGHT_SLASH;
                                    case 48:                    return ZERO;
                                    case 49:                    return ONE;
                                    case 50:                    return TWO;
                                    case 51:                    return THREE;
                                    case 52:                    return FOUR;
                                    case 53:                    return FIVE;
                                    case 54:                    return SIX;
                                    case 55:                    return SEVEN;
                                    case 56:                    return EIGHT;
                                    case 57:                    return NINE;
                                    case 58:                    return COLON;
                                    case 59:                    return SEMICOLON;
                                    case 60:                    return LESS_THAN;
                                    case 61:                    return EQUALS;
                                    case 62:                    return GREATER_THAN;
                                    case 63:                    return QUESTION_MARK;
                                    case 64:                    return AT_SIGN;
                                    case 65:                    return A;
                                    case 66:                    return B;
                                    case 67:                    return C;
                                    case 68:                    return D;
                                    case 69:                    return E;
                                    case 70:                    return F;
                                    case 71:                    return G;
                                    case 72:                    return H;
                                    case 73:                    return I;
                                    case 74:                    return J;
                                    case 75:                    return K;
                                    case 76:                    return L;
                                    case 77:                    return M;
                                    case 78:                    return N;
                                    case 79:                    return O;
                                    case 80:                    return P;
                                    case 81:                    return Q;
                                    case 82:                    return R;
                                    case 83:                    return S;
                                    case 84:                    return T;
                                    case 85:                    return U;
                                    case 86:                    return V;
                                    case 87:                    return W;
                                    case 88:                    return X;
                                    case 89:                    return Y;
                                    case 90:                    return Z;
                                    case 91:                    return LEFT_BRACKET;
                                    case 92:                    return LEFT_SLASH;
                                    case 93:                    return RIGHT_BRACKET;
                                    case 94:                    return CIRCUMFLEX;
                                    case 95:                    return UNDERSCORE;
                                    case 96:                    return GRAVE_ACCENT;
                                    case 123:                   return LEFT_BRACES;
                                    case 124:                   return VERTICAL_BAR;
                                    case 125:                   return RIGHT_BRACES;
                                    case 126:                   return TILDE;
                                    case GLFW_KEY_ESCAPE:       return ESC;
                                    case GLFW_KEY_F1:           return F1;    
                                    case GLFW_KEY_F2:           return F2; 
                                    case GLFW_KEY_F3:           return F3;   
                                    case GLFW_KEY_F4:           return F4;    
                                    case GLFW_KEY_F5:           return F5;   
                                    case GLFW_KEY_F6:           return F6;   
                                    case GLFW_KEY_F7:           return F7;    
                                    case GLFW_KEY_F8:           return F8;
                                    case GLFW_KEY_F9:           return F9;  
                                    case GLFW_KEY_F10:          return F10; 
                                    case GLFW_KEY_F11:          return F11;     
                                    case GLFW_KEY_F12:          return F12;   
                                    case GLFW_KEY_F13:          return F13;     
                                    case GLFW_KEY_F14:          return F14; 
                                    case GLFW_KEY_F15:          return F15;  
                                    case GLFW_KEY_F16:          return F16;      
                                    case GLFW_KEY_F17:          return F17;      
                                    case GLFW_KEY_F18:          return F18;  
                                    case GLFW_KEY_F19:          return F19;      
                                    case GLFW_KEY_F20:          return F20;  
                                    case GLFW_KEY_F21:          return F21; 
                                    case GLFW_KEY_F22:          return F22;       
                                    case GLFW_KEY_F23:          return F23;   
                                    case GLFW_KEY_F24:          return F24;       
                                    case GLFW_KEY_F25:          return F25;   
                                    case GLFW_KEY_UP:           return UP;     
                                    case GLFW_KEY_DOWN:         return DOWN;  
                                    case GLFW_KEY_LEFT:         return LEFT;     
                                    case GLFW_KEY_RIGHT:        return RIGHT;    
                                    case GLFW_KEY_LEFT_SHIFT:   return LEFT_SHIFT;   
                                    case GLFW_KEY_RIGHT_SHIFT:  return RIGHT_SHIFT;   
                                    case GLFW_KEY_LEFT_CONTROL: return LEFT_CTRL;  
                                    case GLFW_KEY_RIGHT_CONTROL:return RIGHT_CTRL;   
                                    case GLFW_KEY_LEFT_ALT:     return LEFT_ALT;  
                                    case GLFW_KEY_RIGHT_ALT:    return RIGHT_ALT;    
                                    case GLFW_KEY_TAB:          return TAB; 
                                    case GLFW_KEY_ENTER:        return ENTER;  
                                    case GLFW_KEY_BACKSPACE:    return BACKSPACE;
                                    case GLFW_KEY_INSERT:       return INSERT;
                                    case GLFW_KEY_DELETE:       return DEL; 
                                    case GLFW_KEY_PAGE_UP:      return PAGEUP;   
                                    case GLFW_KEY_PAGE_DOWN:    return PAGEDOWN; 
                                    case GLFW_KEY_HOME:         return HOME;
                                    case GLFW_KEY_END:          return END;  
                                    case GLFW_KEY_KP_0:         return KP_0;  
                                    case GLFW_KEY_KP_1:         return KP_1;  
                                    case GLFW_KEY_KP_2:         return KP_2; 
                                    case GLFW_KEY_KP_3:         return KP_3; 
                                    case GLFW_KEY_KP_4:         return KP_4; 
                                    case GLFW_KEY_KP_5:         return KP_5; 
                                    case GLFW_KEY_KP_6:         return KP_6;
                                    case GLFW_KEY_KP_7:         return KP_7;  
                                    case GLFW_KEY_KP_8:         return KP_8;
                                    case GLFW_KEY_KP_9:         return KP_9; 
                                    case GLFW_KEY_KP_DIVIDE:    return KP_DIVIDE;
                                    case GLFW_KEY_KP_MULTIPLY:  return KP_MULTIPLY;
                                    case GLFW_KEY_KP_SUBTRACT:  return KP_SUBTRACT;
                                    case GLFW_KEY_KP_ADD:       return KP_ADD;
                                    case GLFW_KEY_KP_DECIMAL:   return KP_DECIMAL;
                                    case GLFW_KEY_KP_EQUAL:     return KP_EQUAL;
                                    case GLFW_KEY_KP_ENTER:     return KP_ENTER;
                                    case GLFW_KEY_NUM_LOCK:    return KP_NUM_LOCK;
                                    case GLFW_KEY_CAPS_LOCK:    return CAPS_LOCK;
                                    case GLFW_KEY_SCROLL_LOCK:  return SCROLL_LOCK;
                                    case GLFW_KEY_PAUSE:        return PAUSE;
                                    case GLFW_KEY_MENU:         return MENU;
                                    default:                    return UNKNOWN;
                                };
                            }(),
                            scancode,
                            [&]()
                            {
                                switch (action)
                                {
                                    case GLFW_PRESS:    return KeyEvent::STATE_PRESSED;
                                    case GLFW_RELEASE:  return KeyEvent::STATE_RELEASED;
                                    case GLFW_REPEAT:   return KeyEvent::STATE_REPEAT;
                                    default:            return KeyEvent::STATE_UNKNOWN;
                                }
                            }(),
                            [&]()
                            {
                                switch (mods)
                                {
                                    case GLFW_PRESS:    return KeyEvent::MODIFIER_SHIFT;
                                    case GLFW_RELEASE:  return KeyEvent::MODIFIER_CONTROL;
                                    case GLFW_REPEAT:   return KeyEvent::MODIFIER_ALT;
                                    default:            return KeyEvent::MODIFIER_NONE;
                                }
                            }()
                            ));
    }
}