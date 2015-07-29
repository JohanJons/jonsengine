#include "include/Core/Logging/LogManager.h"

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
    static LogManager gLogManager(gGameEngineTag);


    LogManager::LogManager(const std::string logPrefix) : mLogPrefix(logPrefix), mLogStream(&mStreamBuf), mLogPath(InternalGetLogName()), mLogFilter(LogLevel::Info)
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

    void LogManager::SetLevelFilter(const LogLevel level)
    {
        mLogFilter = level;
    }


    std::string LogManager::InternalGetLogName() const
    {
        std::stringstream ret;

        ret << mLogPrefix;
        ret << "_Log";
        ret << ".txt";

        return ret.str();
    }

    std::string LogManager::LogLevelToString(const LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::Debug:       {   return "DEBUG";     break;  }
            case LogLevel::Info:        {   return "INFO";      break;  }
            case LogLevel::Warning:     {   return "WARNING";   break;  }
            case LogLevel::Error:       {   return "ERROR";     break;  }
            default:                    {   return "UNKNOWN";   break;  }
        }
    }
}