#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Logging/JonsStreamBuf.h"
#include "include/Core/Logging/JonsOutputStream.h"
#include "include/Core/Threading/ScopedLock.h"

#include "include/Core/Utils/JonsTime.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace JonsEngine
{
    /* LogManager definition */
    class LogManager
    {
    public:
        enum LogLevel
        {
            LEVEL_DEBUG = 0,
            LEVEL_INFO,
            LEVEL_WARNING,
            LEVEL_ERROR,
            LEVEL_CRITICAL
        };

        LogManager();
        ~LogManager();

        void Log(LogLevel level, const std::string& logMsg);

        void AddOutputStream(std::streambuf* const sb);
        void RemoveOutputStream(std::streambuf* const sb);
        bool IsOutputStreamAdded(std::streambuf* const sb);

        const std::string& GetFileLogPath() const;

        void SetLevelFilter(LogLevel level);
        LogLevel GetLevelFilter() const;

    private:
        const std::string InternalGetLogName() const;
        const std::string LogLevelToString(LogLevel level);

        std::ostream mLogStream;
        std::string mLogPath;
        std::ofstream mFileStream;
        JonsStreamBuf mStreamBuf;
        LogLevel  mLogFilter;
        Mutex mMutex;

    };


    /* LogManager inlines */
    inline const std::string& LogManager::GetFileLogPath() const           { return mLogPath;      }
    inline LogManager::LogLevel LogManager::GetLevelFilter() const         { return mLogFilter;    }


    /* LogManager free functions */
    namespace Globals
    {
        LogManager& GetLogManager();
    }
}
