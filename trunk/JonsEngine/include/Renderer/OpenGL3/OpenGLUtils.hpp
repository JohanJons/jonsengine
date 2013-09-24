#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include "boost/current_function.hpp"
#include <sstream>
#include <exception>

namespace JonsEngine
{
    #define CHECK_GL_ERROR(Logger)                                                                                                            \
    {                                                                                                                                         \
        if (is_debug)                                                                                                                         \
        {                                                                                                                                     \
            GLuint err = glGetError();                                                                                                        \
            if (err != GL_NO_ERROR)                                                                                                           \
            {                                                                                                                                 \
                std::stringstream errorSS;                                                                                                    \
                errorSS << "OpenGL Error: (" << __FILE__ << ":" << __LINE__ << ") in " << BOOST_CURRENT_FUNCTION << ": glGetError = " << err; \
                Logger.Log(LogManager::LEVEL_ERROR, errorSS.str());                                                                           \
                throw new std::runtime_error(errorSS.str());                                                                                  \
            }                                                                                                                                 \
        }                                                                                                                                     \
    }
}