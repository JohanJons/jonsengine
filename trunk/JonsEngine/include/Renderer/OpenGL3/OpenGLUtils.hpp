#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include "boost/current_function.hpp"
#include <sstream>
#include <exception>

namespace JonsEngine
{
    #ifdef _DEBUG
    #define GLCALL(function)                                                                                                                            \
    {                                                                                                                                                   \
        function;                                                                                                                                       \
        GLuint err = glGetError();                                                                                                                      \
        if (err != GL_NO_ERROR)                                                                                                                         \
        {                                                                                                                                               \
            auto parseGLError = [](const GLuint err)                                                                                                    \
                            {                                                                                                                           \
                                switch (err)                                                                                                            \
                                {                                                                                                                       \
                                    case GL_INVALID_ENUM:      return std::string("GL_INVALID_ENUM");                                                                \
                                    case GL_INVALID_VALUE:     return std::string("GL_INVALID_VALUE");                                                               \
                                    case GL_INVALID_OPERATION: return std::string("GL_INVALID_OPERATION");                                                           \
                                    default:                   return std::to_string(err);                                                                              \
                                }                                                                                                                       \
                                                                                                                                                        \
                            };                                                                                                                          \
            std::stringstream errorSS;                                                                                                                  \
            errorSS << "OpenGL Error: (" << __FILE__ << ":" << __LINE__ << ") in " << BOOST_CURRENT_FUNCTION << ": glGetError = " << parseGLError(err); \
            Logger::GetRendererLogger().Log(LogManager::LEVEL_ERROR, errorSS.str());                                                                    \
            throw new std::runtime_error(errorSS.str());                                                                                                \
        }                                                                                                                                               \
    }
    #else
    #define GLCALL(function, Logger) function;
    #endif
}