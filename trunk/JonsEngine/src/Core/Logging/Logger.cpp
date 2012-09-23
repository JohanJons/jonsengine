#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
	//
	// Default globally accessable logger
	//
	static Logger gLogger;

	Logger& GetGlobalLogger()
	{
		return gLogger;
	}



	//
	// LoggerImpl implementation
	//
	Logger::Logger() : mStreamBuf(), mFileStream(), mLogStream(&mStreamBuf), mLogPath(InternalGetLogName())
	{
	}

	Logger::~Logger()
	{
		Destroy();
	}

	void Logger::Init()
	{
		mFileStream.open(mLogPath);

		AddStream(mFileStream.rdbuf());
	}

	void Logger::Destroy()
	{
		if (mFileStream.is_open())
			mFileStream.close();
	}

	void Logger::AddStream(std::streambuf* const sb)
	{
		if (sb)
			mStreamBuf.AddStream(sb);
	}

	void Logger::RemoveStream(std::streambuf* const sb)
	{
		if (sb)
			mStreamBuf.RemoveStream(sb);
	}

	bool Logger::IsStreamAdded(std::streambuf* const sb) const
	{
		if (sb)
			return mStreamBuf.IsStreamAdded(sb);
		else
			return false;
	}

	const std::string& Logger::GetFileLogPath() const
	{
		return mLogPath;
	}

	const std::string Logger::InternalGetLogName() const
	{
		std::stringstream ret;

		ret << EngineLogTag;
		ret << "_"; ret << GetHour();
		ret << "_"; ret << GetMinute();
		ret << "_";
		ret << "_"; ret << GetDay();
		ret << "_"; ret << GetMonth();
		ret << "_"; ret << GetYear();
		ret << ".txt";

		return ret.str();
	}
	
	std::ostream& Logger::LogInfo()
	{
		mLogStream << EngineLogTag << "_INFO: ";

		return mLogStream;
	}

	std::ostream& Logger::LogDebug()
	{
		mLogStream << EngineLogTag << "_DEBUG: ";

		return mLogStream;
	}

	std::ostream& Logger::LogWarn()
	{
		mLogStream << EngineLogTag << "_WARN: ";

		return mLogStream;
	}

	std::ostream& Logger::LogError()
	{
		mLogStream << EngineLogTag << "_ERROR: ";

		return mLogStream;
	}

}
