#pragma once

#include "include/Core/Logging/LogManager.h"

#include <string>


#define JONS_LOG_DEBUG(logger, logMsg)          { logger.Log(LogManager::LEVEL_DEBUG, logMsg);    }
#define JONS_LOG_INFO(logger, logMsg)           { logger.Log(LogManager::LEVEL_INFO, logMsg);     }
#define JONS_LOG_WARNING(logger, logMsg)        { logger.Log(LogManager::LEVEL_WARNING, logMsg);  }
#define JONS_LOG_ERROR(logger, logMsg)          { logger.Log(LogManager::LEVEL_ERROR, logMsg);    }
#define JONS_LOG_CRITICAL(logger, logMsg)       { logger.Log(LogManager::LEVEL_CRITICAL, logMsg); }


namespace JonsEngine
{
    /*
     * Object used to log to the default logmanager 
     */

    /* Logger class definition */
    class Logger
    {
    public:
        Logger(const std::string& loggerName);
        ~Logger();
        static Logger& GetCoreLogger();
        static Logger& GetVideoLogger();
        static Logger& GetInputLogger();

        void Log(LogManager::LogLevel level, const std::string& logMsg);

        const std::string& GetLoggerName() const;


    private:
        const std::string mLoggerName;
    };


    /* Logger Inlines */
    inline const std::string& Logger::GetLoggerName() const     { return mLoggerName; }
}
