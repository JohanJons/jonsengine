#ifndef _LOG_MANAGER_IMPL_H
#define _LOG_MANAGER_IMPL_H

#ifdef ANDROID
#include "jni.h"
#include "AndroidLogStream.h"
#endif

#include "../../../Interface/ILogManager.h"
#include "../Utils/JonsTime.h"
#include "JonsStreamBuf.h"
#include "JonsOutputStream.h"
#include "DummyLogStream.h"

#include "../../../Interface/EngineDefs.h"


namespace JonsEngine
{
	class LogManagerImpl : public ILogManager
	{
	public:
		#ifdef ANDROID
			LogManagerImpl(JNIEnv* env);
		#else
			LogManagerImpl();
		#endif
		~LogManagerImpl();

		int32_t Init();
		int32_t Init(bool LogToFile, bool LogToStdOut);
		bool Destroy();
		bool Start();
		bool Stop();
		void Tick();
		bool isRunning();

		void AddStream(std::streambuf* sb);
		void RemoveStream(std::streambuf* sb);
		bool IsStreamAdded(std::streambuf* sb);
		std::string GetLogName();
		std::string GetLogPath();

		std::ostream& LogInfo();
		std::ostream& LogDebug();
		std::ostream& LogWarn();
		std::ostream& LogError();

	private:
		void BaseConstructor();

		bool mRunning;
		bool mLogToFileDefault;
		bool mLogToOSDefault;
		std::ofstream* mFileStream;
		JonsStreamBuf* mStreamBuf;
		DummyLogStreamBuf* mDummyStreamBuf;

		#ifdef ANDROID
			JNIEnv* mJNIEnv;
			AndroidLogStreamBuf* mAndroidLogBuf;
		#endif

		// REDIRECTER

	};

}

#endif
