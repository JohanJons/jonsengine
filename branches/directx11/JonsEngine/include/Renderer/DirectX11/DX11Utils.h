#pragma once

#include "include/Core/Logging/Logger.h"

#include "boost/current_function.hpp"
#include <d3d11.h>
#include <sstream>
#include <exception>

namespace JonsEngine
{
    const float Z_NEAR = 0.1f;
    const float Z_FAR = 100.0f;

    #ifdef _DEBUG
    #define DXCALL(function)                                                                                                                            \
    {                                                                                                                                                   \
        HRESULT result = function;                                                                                                                      \
        if (FAILED(result))                                                                                                                             \
        {                                                                                                                                               \
            std::stringstream errorSS;                                                                                                                  \
            errorSS << "D3D11 Error: (" << __FILE__ << ":" << __LINE__ << ") in " << BOOST_CURRENT_FUNCTION << ": code " << result;                     \
            Logger::GetRendererLogger().Log(LogManager::LEVEL_ERROR, errorSS.str());                                                                    \
            throw new std::runtime_error(errorSS.str());                                                                                                \
        }                                                                                                                                               \
    }
    #else
    #define DXCALL(function) function;
    #endif
}