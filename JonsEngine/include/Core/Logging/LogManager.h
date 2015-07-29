#pragma once

#include "include/Core/Types.h"
#include "include/Core/Logging/StreamBuf.h"
#include "include/Core/Logging/OutputStream.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace JonsEngine
{
    class LogManager
    {
    public:
        enum class LogLevel
        {
            Debug,
            Info,
            Warning,
            Error
        };

        LogManager(const std::string logPrefix);
        ~LogManager();

        static LogManager& GetDefaultLogManager();

        void Log(LogLevel level, const std::string& logMsg);

        void AddOutputStream(std::streambuf* const sb);
        void RemoveOutputStream(std::streambuf* const sb);
        bool IsOutputStreamAdded(std::streambuf* const sb);

        const std::string& GetFileLogPath() const;

        void SetLevelFilter(const LogLevel level);
        LogLevel GetLevelFilter() const;


    private:
        std::string InternalGetLogName() const;
        std::string LogLevelToString(const LogLevel level) const;

        const std::string mLogPrefix;
        std::ostream mLogStream;
        std::string mLogPath;
        std::ofstream mFileStream;
        StreamBuf mStreamBuf;
        LogLevel mLogFilter;
    };


    /* LogManager inlines */
    inline const std::string& LogManager::GetFileLogPath() const           { return mLogPath;      }
    inline LogManager::LogLevel LogManager::GetLevelFilter() const         { return mLogFilter;    }
}
