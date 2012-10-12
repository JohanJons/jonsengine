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
	class Logger
	{
	public:
		Logger(const std::string& loggerName);
		~Logger();

		void Log(LogManager::LogLevel level, const std::string& logMsg);

		inline const std::string& GetLoggerName() const		{ return mLoggerName; }


	private:
		const std::string mLoggerName;
	};

	namespace Globals
	{
		Logger& GetEngineLogger();
		Logger& GetVideoLogger();
		Logger& GetInputLogger();
		Logger& GetThreadingLogger();
	}
}
