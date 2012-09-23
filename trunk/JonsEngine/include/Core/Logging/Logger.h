#pragma once

#include "interface/Core/EngineDefs.h"

#include "include/Core/Logging/JonsStreamBuf.h"
#include "include/Core/Logging/JonsOutputStream.h"

#include "include/Core/Utils/JonsTime.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace JonsEngine
{
	class Logger
	{
	public:
		Logger();
		~Logger();

		void AddStream(std::streambuf* const sb);
		void RemoveStream(std::streambuf* const sb);
		bool IsStreamAdded(std::streambuf* const sb) const;
		const std::string& GetFileLogPath() const;

		std::ostream& LogInfo();
		std::ostream& LogDebug();
		std::ostream& LogWarn();
		std::ostream& LogError();

	private:
		void Init();
		void Destroy();

		const std::string InternalGetLogName() const;
		const std::string InternalGetLogPath() const;

		std::ostream mLogStream;
		std::string mLogPath;
		std::ofstream mFileStream;
		JonsStreamBuf mStreamBuf;

	};

	Logger& GetGlobalLogger();
}
