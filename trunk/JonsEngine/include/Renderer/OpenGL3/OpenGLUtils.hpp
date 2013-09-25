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
    #define CHECK_GL_ERROR(Logger)                                                                                                        \
    {                                                                                                                                     \
        GLuint err = glGetError();                                                                                                        \
        if (err != GL_NO_ERROR)                                                                                                           \
        {                                                                                                                                 \
            std::stringstream errorSS;                                                                                                    \
            errorSS << "OpenGL Error: (" << __FILE__ << ":" << __LINE__ << ") in " << BOOST_CURRENT_FUNCTION << ": glGetError = " << err; \
            Logger.Log(LogManager::LEVEL_ERROR, errorSS.str());                                                                           \
            throw new std::runtime_error(errorSS.str());                                                                                  \
        }                                                                                                                                 \
    }
    #else
    #define CHECK_GL_ERROR(Logger)
    #endif
}