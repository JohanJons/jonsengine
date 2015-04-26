#pragma once

#include "include/Core/Logging/Logger.h"

#include <exception>

#define JONS_ERROR_LEVEL_WARNING 0
#define JONS_ERROR_LEVEL_ERROR 1

#ifndef JONS_ERROR_LEVEL
    // non-standard debug macro... problem?
    #if _DEBUG
        #define JONS_ERROR_LEVEL JONS_ERROR_LEVEL_WARNING
    #else
        #define JONS_ERROR_LEVEL JONS_ERROR_LEVEL_ERROR
    #endif
#endif

#define JONS_ERROR(logger, msg)     { logger.Log(LogManager::LEVEL_ERROR, msg); if (JONS_ERROR_LEVEL >= JONS_ERROR_LEVEL_ERROR) throw std::runtime_error(msg);  }
#define JONS_WARNING(logger, msg)   { logger.Log(LogManager::LEVEL_WARNING, msg); if (JONS_ERROR_LEVEL >= JONS_ERROR_LEVEL_WARNING) throw std::runtime_error(msg);  }