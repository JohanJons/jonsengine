#pragma once

#include "include/Core/Logging/LogManager.h"

#include <string>


#define JONS_LOG_DEBUG(logger, logMsg)			{ logger.Log(LogManager::LEVEL_DEBUG, logMsg);	  }
#define JONS_LOG_INFO(logger, logMsg)			{ logger.Log(LogManager::LEVEL_INFO, logMsg);	  }
#define JONS_LOG_WARNING(logger, logMsg)		{ logger.Log(LogManager::LEVEL_WARNING, logMsg);  }
#define JONS_LOG_ERROR(logger, logMsg)			{ logger.Log(LogManager::LEVEL_ERROR, logMsg);	  }
#define JONS_LOG_CRITICAL(logger, logMsg)		{ logger.Log(LogManager::LEVEL_CRITICAL, logMsg); }


namespace JonsEngine
{
    /* Logger class definition */
    class Logger
    {
    public:
        Logger(const std::string& loggerName);
        ~Logger();

        void Log(LogManager::LogLevel level, const std::string& logMsg);

        const std::string& GetLoggerName() const;


    private:
        const std::string mLoggerName;
    };


    /* Logger Inlines */
    inline const std::string& Logger::GetLoggerName() const     { return mLoggerName; }


    /* Logger free functions */
    namespace Globals
    {
        Logger& GetEngineLogger();
        Logger& GetVideoLogger();
        Logger& GetInputLogger();
        Logger& GetThreadingLogger();
    }
}
