#ifndef _LOG_MANAGER_IMPL_H
#define _LOG_MANAGER_IMPL_H

#include <string>

#ifdef ANDROID
#include "jni.h"
#include "include/Core/Logging/AndroidLogStream.h"
#endif

#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Memory/IMemoryManager.h"
#include "interface/Core/EngineDefs.h"

#include "include/Core/Logging/JonsStreamBuf.h"
#include "include/Core/Logging/JonsOutputStream.h"
#include "include/Core/Logging/DummyLogStream.h"

#include "include/Core/Utils/JonsTime.h"


namespace JonsEngine
{
	class LogManagerImpl : public ILogManager
	{
	public:
		LogManagerImpl();
		~LogManagerImpl();

		#ifdef ANDROID
			bool Init(bool LogToFile, bool LogToStdOut, const std::string& absFilePath, JNIEnv* env);
		#endif
		bool Init(bool LogToFile, bool LogToStdOut, const std::string& absFilePath);
		bool Destroy();
		bool Start();
		bool Stop();
		void Tick();
		bool isRunning();

		void AddStream(std::streambuf* const sb);
		void RemoveStream(std::streambuf* const sb);
		bool IsStreamAdded(std::streambuf* const sb) const;
		const std::string& GetFileLogPath() const;

		std::ostream& LogInfo();
		std::ostream& LogDebug();
		std::ostream& LogWarn();
		std::ostream& LogError();

	private:
		const std::string InternalGetLogName() const;
		const std::string InternalGetLogPath() const;

		bool mRunning;
		bool mInitialized;
		bool mLogToFileDefault;
		bool mLogToOSDefault;
		std::ostream mLogStream;
		std::string mLogPath;
		std::ofstream mFileStream;
		JonsStreamBuf mStreamBuf;
		DummyLogStreamBuf mDummyStreamBuf;

		#ifdef ANDROID
			JNIEnv* mJNIEnv;
			AndroidLogStreamBuf mAndroidLogBuf;
		#endif

	};

}

#endif
