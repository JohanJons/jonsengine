#pragma once

#include "LogManager.h"

#include <string>


#define JONS_LOG_DEBUG(logger, logMsg)          { logger.Log(LogManager::LogLevel::Debug, logMsg);    }
#define JONS_LOG_INFO(logger, logMsg)           { logger.Log(LogManager::LogLevel::Info, logMsg);     }
#define JONS_LOG_WARNING(logger, logMsg)        { logger.Log(LogManager::LogLevel::Warning, logMsg);  }
#define JONS_LOG_ERROR(logger, logMsg)          { logger.Log(LogManager::LogLevel::Error, logMsg);    }

namespace JonsEngine
{
    /*
     * Object used to log to the default logmanager 
     */
    class Logger
    {
    public:
		Logger();
        Logger(const std::string& loggerName);
        ~Logger();

        static Logger& GetCoreLogger();
        static Logger& GetWindowLogger();
        static Logger& GetRendererLogger();

        void Log(const LogManager::LogLevel level, const std::string& logMsg);

        
        const std::string mLoggerName;
    };
}
