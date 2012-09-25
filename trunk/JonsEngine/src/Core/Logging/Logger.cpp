#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
	Logger::Logger(const std::string& loggerName) : mLoggerName(loggerName) 
	{
	}
		
	Logger::~Logger() 
	{
	}
		
	void Logger::Log(LogManager::LogLevel level, const std::string& logMsg)
	{
		const std::string msg = mLoggerName + ": " +  logMsg;

		LogManager::Instance().Log(level, msg);
	}
}