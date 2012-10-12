#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
	/////////////////////////////////////////////////////////////////
	/////				Global logger definitions				/////
	/////////////////////////////////////////////////////////////////

	namespace Globals
	{
		static Logger gEngineLogger("ENGINE");
		static Logger gVideoLogger("VIDEO");
		static Logger gInputLogger("INPUT");
		static Logger gThreadingLogger("THREADING");

		Logger& GetEngineLogger()			{ return gEngineLogger;		}
		Logger& GetVideoLogger()			{ return gVideoLogger;		}
		Logger& GetInputLogger()			{ return gInputLogger;		}
		Logger& GetThreadingLogger()		{ return gThreadingLogger;	}

	}



	/////////////////////////////////////////////////////////////
	/////				Logger implementation				/////
	/////////////////////////////////////////////////////////////

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