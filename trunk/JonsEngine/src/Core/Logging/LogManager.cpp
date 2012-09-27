#include "include/Core/Logging/LogManager.h"

namespace JonsEngine
{
	// Defintion of engine log tags
	const std::string GameEngineTag		=		"JonsEngine";
	const std::string EngineTag			=		"ENGINE";
	const std::string ThreadingTag		=		"THREADING";


	// LogManager singleton
	LogManager LogManager::mInstance;
	LogManager& LogManager::Instance()
	{
		return mInstance;
	}


	LogManager::LogManager() : mStreamBuf(), mFileStream(), mLogStream(&mStreamBuf), mLogPath(InternalGetLogName()), mLogFilter(LEVEL_INFO)
	{
		Init();
	}

	LogManager::~LogManager()
	{
		Destroy();
	}

	void LogManager::Init()
	{
		mFileStream.open(mLogPath);

		AddOutputStream(mFileStream.rdbuf());
	}

	void LogManager::Destroy()
	{
		if (mFileStream.is_open())
			mFileStream.close();
	}

	void LogManager::Log(LogLevel level, const std::string& logMsg)
	{
		if (level < mLogFilter)
			return;

		mLogStream << GameEngineTag << " - " + LogLevelToString(level) + " - " << logMsg << std::endl;
	}

	void LogManager::AddOutputStream(std::streambuf* const sb)
	{
		if (sb)
			mStreamBuf.AddStream(sb);
	}

	void LogManager::RemoveOutputStream(std::streambuf* const sb)
	{
		if (sb)
			mStreamBuf.RemoveStream(sb);
	}

	bool LogManager::IsOutputStreamAdded(std::streambuf* const sb) const
	{
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
		ret << "_"; ret << GetHour();
		ret << "_"; ret << GetMinute();
		ret << "_";
		ret << "_"; ret << GetDay();
		ret << "_"; ret << GetMonth();
		ret << "_"; ret << GetYear();
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
