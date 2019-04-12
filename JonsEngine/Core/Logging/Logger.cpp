#include "Logger.h"

namespace JonsEngine
{
	static const std::string gDefaultLoggerName = "DefaultLogger";

    static Logger gCoreLogger("CORE");
    static Logger gRendererLogger("RENDERER");
    static Logger gWindowLogger("WINDOW");


	Logger::Logger() : Logger(gDefaultLoggerName)
	{
	}

    Logger::Logger(const std::string& loggerName) : mLoggerName(loggerName) 
    {
    }
        
    Logger::~Logger() 
    {
    }


    Logger& Logger::GetCoreLogger()
    { 
        return gCoreLogger;
    }

    Logger& Logger::GetRendererLogger()
    { 
        return gRendererLogger;
    }

    Logger& Logger::GetWindowLogger()
    {
        return gWindowLogger;
    }


    void Logger::Log(const LogManager::LogLevel level, const std::string& logMsg)
    {
        const std::string msg = mLoggerName + ": " +  logMsg;

        LogManager::GetDefaultLogManager().Log(level, msg);
    }
}