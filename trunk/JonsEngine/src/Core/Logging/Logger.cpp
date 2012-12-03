#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    static Logger gCoreLogger("CORE");
    static Logger gVideoLogger("VIDEO");
    static Logger gInputLogger("INPUT");


    Logger::Logger(const std::string& loggerName) : mLoggerName(loggerName) 
    {
    }
        
    Logger::~Logger() 
    {
    }

    Logger& Logger::GetCoreLogger()     { return gCoreLogger;       }
    Logger& Logger::GetVideoLogger()    { return gVideoLogger;      } 
    Logger& Logger::GetInputLogger()    { return gInputLogger;      }
        
    void Logger::Log(LogManager::LogLevel level, const std::string& logMsg)
    {
        const std::string msg = mLoggerName + ": " +  logMsg;

        LogManager::GetDefaultLogManager().Log(level, msg);
    }
}