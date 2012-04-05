#ifndef _I_LOG_MANAGER_H
#define _I_LOG_MANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef ANDROID
#include <android/log.h>
#endif

#include "EngineDefs.h"

namespace JonsEngine
{

	class ILogManager
	{
	public:
		virtual ~ILogManager() { }

		virtual int32_t Init() = 0;
		virtual int32_t Init(bool LogToFile, bool LogToStdOut) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		virtual void AddStream(std::streambuf* sb) = 0;
		virtual void RemoveStream(std::streambuf* sb) = 0;
		virtual bool IsStreamAdded(std::streambuf* sb) = 0;

		virtual std::ostream& LogInfo() = 0;
		virtual std::ostream& LogDebug() = 0;
		virtual std::ostream& LogWarn() = 0;
		virtual std::ostream& LogError() = 0;

	protected:
		std::ostream* mLogStream;
	};

}

#endif
