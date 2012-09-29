#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Logging/JonsStreamBuf.h"
#include "include/Core/Logging/JonsOutputStream.h"

#include "include/Core/Utils/JonsTime.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// TODO - Add multithread support

namespace JonsEngine
{
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

		static LogManager& Instance();
		~LogManager();

		void Log(LogLevel level, const std::string& logMsg);

		void AddOutputStream(std::streambuf* const sb);
		void RemoveOutputStream(std::streambuf* const sb);
		bool IsOutputStreamAdded(std::streambuf* const sb) const;

		const std::string& GetFileLogPath() const;

		void SetLevelFilter(LogLevel level);
		LogLevel GetLevelFilter() const;

	private:
		LogManager();

		void Init();
		void Destroy();

		const std::string InternalGetLogName() const;
		const std::string LogLevelToString(LogLevel level);

		static LogManager mInstance;
		std::ostream mLogStream;
		std::string mLogPath;
		std::ofstream mFileStream;
		JonsStreamBuf mStreamBuf;
		LogLevel  mLogFilter;

	};
}