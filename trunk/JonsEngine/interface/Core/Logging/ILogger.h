#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace JonsEngine
{
	class ILogger
	{
	public:
		virtual ~ILogger() { }

		virtual void AddStream(std::streambuf* const sb) = 0;
		virtual void RemoveStream(std::streambuf* const sb) = 0;
		virtual bool IsStreamAdded(std::streambuf* const sb) const = 0;
		virtual const std::string& GetFileLogPath() const = 0;

		virtual std::ostream& LogInfo() = 0;
		virtual std::ostream& LogDebug() = 0;
		virtual std::ostream& LogWarn() = 0;
		virtual std::ostream& LogError() = 0;
	};

	namespace Globals
	{
		ILogger& GetDefaultLogger();
	}
}