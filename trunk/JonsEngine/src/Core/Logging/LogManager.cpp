#include "include/Core/Logging/LogManager.h"

namespace JonsEngine
{
	// Defintion of engine log tags
	const std::string GameEngineTag		=		"JonsEngine";

	// LogManager singleton
	LogManager LogManager::mInstance;
	LogManager& LogManager::Instance()
	{
		return mInstance;
	}


	LogManager::LogManager() : mStreamBuf(), mFileStream(), mLogStream(&mStreamBuf), mLogPath(InternalGetLogName()), mLogFilter(LEVEL_INFO), mMutex()
	{
		Init();
	}

	LogManager::~LogManager()
	{
		Destroy();
	}

	void LogManager::Init()
	{
		ScopedLock lock(mMutex);

		mFileStream.open(mLogPath, std::ifstream::trunc);

		AddOutputStream(mFileStream.rdbuf());
	}

	void LogManager::Destroy()
	{
		ScopedLock lock(mMutex);

		if (mFileStream.is_open())
			mFileStream.close();
	}

	void LogManager::Log(LogLevel level, const std::string& logMsg)
	{
		ScopedLock lock(mMutex);

		if (level < mLogFilter)
			return;

		mLogStream << GameEngineTag << " - " + LogLevelToString(level) + " - " << logMsg << std::endl;
	}

	void LogManager::AddOutputStream(std::streambuf* const sb)
	{
		ScopedLock lock(mMutex);

		if (sb)
			mStreamBuf.AddStream(sb);
	}

	void LogManager::RemoveOutputStream(std::streambuf* const sb)
	{
		ScopedLock lock(mMutex);

		if (sb)
			mStreamBuf.RemoveStream(sb);
	}

	bool LogManager::IsOutputStreamAdded(std::streambuf* const sb)
	{
		ScopedLock lock(mMutex);

		if (sb)
			return mStreamBuf.IsStreamAdded(sb);
		else
			return false;
	}

	const std::string& LogManager::GetFileLogPath() const
	{
		return mLogPath;
	}

	void LogManager::SetLevelFilter(LogLevel level)
	{
		ScopedLock lock(mMutex);

		mLogFilter = level;
	}
	
	LogManager::LogLevel LogManager::GetLevelFilter() const
	{
		return mLogFilter;
	}

	const std::string LogManager::InternalGetLogName() const
	{
		std::stringstream ret;

		ret << GameEngineTag;
		ret << "_Log";
		ret << ".txt";

		return ret.str();
	}

	const std::string LogManager::LogLevelToString(LogLevel level)
	{
		switch (level)
		{
			case LEVEL_DEBUG:		{	return "DEBUG";		break;	}
			case LEVEL_INFO:		{	return "INFO";		break;	}
			case LEVEL_WARNING:		{	return "WARNING";	break;	}
			case LEVEL_ERROR:		{	return "ERROR";		break;	}
			case LEVEL_CRITICAL:	{	return "CRITICAL";	break;	}
			default:				{	return "UNKNOWN";	break;	}
		}

	}
}
