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

        glfwEnable(GLFW_KEY_REPEAT);
        gGLFWInputBackendInstance = this;
    }
        
    GLFWInputBackend::~GLFWInputBackend()
    {
        glfwSetKeyCallback(NULL);
        glfwSetMouseButtonCallback(NULL);
        glfwSetMousePosCallback(NULL);
        glfwEnable(GLFW_MOUSE_CURSOR);

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
        glfwSetKeyCallback(glfwOnKey);
    }

    bool GLFWInputBackend::IsMouseButtonCallbackSet() const                               { return mOnMouseButton != NULL;    }
    bool GLFWInputBackend::IsMouseMotionCallbackSet() const                               { return mOnMouseMotion != NULL;    }
    bool GLFWInputBackend::IsKeyCallbackSet() const                                       { return mOnKey != NULL;            }


    void GLFWInputBackend::ShowMouseCursor(bool show)
    {
        glfwDisable(GLFW_MOUSE_CURSOR);
    }
        
    void GLFWInputBackend::SetMousePosition(uint32_t x, uint32_t y)
    {
        glfwSetMousePos(x, y);
    }


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
            KeyEvent ev(ParseGLFWKey(key), action == GLFW_PRESS ? KeyEvent::PRESSED : KeyEvent::RELEASED);

            gGLFWInputBackendInstance->mOnKey(ev);
        }
    }

    Key GLFWInputBackend::ParseGLFWKey(int key)
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
        case GLFW_KEY_ESC:          return ESC;
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
        case GLFW_KEY_LSHIFT:       return LSHIFT;   
        case GLFW_KEY_RSHIFT:       return RSHIFT;   
        case GLFW_KEY_LCTRL:        return LCTRL;  
        case GLFW_KEY_RCTRL:        return RCTRL;   
        case GLFW_KEY_LALT:         return LALT;  
        case GLFW_KEY_RALT:         return RALT;    
        case GLFW_KEY_TAB:          return TAB; 
        case GLFW_KEY_ENTER:        return ENTER;  
        case GLFW_KEY_BACKSPACE:    return BACKSPACE;
        case GLFW_KEY_INSERT:       return INSERT;
        case GLFW_KEY_DEL:          return DEL; 
        case GLFW_KEY_PAGEUP:       return PAGEUP;   
        case GLFW_KEY_PAGEDOWN:     return PAGEDOWN; 
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
        case GLFW_KEY_KP_NUM_LOCK:  return KP_NUM_LOCK;
        case GLFW_KEY_CAPS_LOCK:    return CAPS_LOCK;   
        case GLFW_KEY_SCROLL_LOCK:  return SCROLL_LOCK; 
        case GLFW_KEY_PAUSE:        return PAUSE;    
        case GLFW_KEY_LSUPER:       return LSUPER;  
        case GLFW_KEY_RSUPER:       return RSUPER;  
        case GLFW_KEY_MENU:         return MENU; 

        default:
            return UNKNOWN;
        };
    }
}