#include "include/Core/Logging/LogManager.h"


namespace JonsEngine
{
    static LogManager gLogManager(GameEngineTag);


    LogManager::LogManager(const std::string logPrefix) : mLogPrefix(logPrefix), mLogStream(&mStreamBuf), mLogPath(InternalGetLogName()), mLogFilter(LEVEL_INFO)
    {
        mFileStream.open(mLogPath, std::ifstream::trunc);

        AddOutputStream(mFileStream.rdbuf());
    }

    LogManager::~LogManager()
    {
        if (mFileStream.is_open())
            mFileStream.close();
    }

    LogManager& LogManager::GetDefaultLogManager()
    {
        return gLogManager;
    }

    void LogManager::Log(LogLevel level, const std::string& logMsg)
    {
        if (level < mLogFilter)
            return;

        mLogStream << mLogPrefix << " - " + LogLevelToString(level) + " - " << logMsg << std::endl;
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

    bool LogManager::IsOutputStreamAdded(std::streambuf* const sb)
    {
        if (sb)
            return mStreamBuf.IsStreamAdded(sb);
        else
            return false;
    }

    void LogManager::SetLevelFilter(LogLevel level)
    {
        mLogFilter = level;
    }

    const std::string LogManager::InternalGetLogName() const
    {
        std::stringstream ret;

        ret << mLogPrefix;
        ret << "_Log";
        ret << ".txt";

        return ret.str();
    }

    const std::string LogManager::LogLevelToString(LogLevel level)
    {
        switch (level)
        {
            case LEVEL_DEBUG:       {   return "DEBUG";     break;  }
            case LEVEL_INFO:        {   return "INFO";      break;  }
            case LEVEL_WARNING:     {   return "WARNING";   break;  }
            case LEVEL_ERROR:       {   return "ERROR";     break;  }
            case LEVEL_CRITICAL:    {   return "CRITICAL";  break;  }
            default:                {   return "UNKNOWN";   break;  }
        }
    }
}