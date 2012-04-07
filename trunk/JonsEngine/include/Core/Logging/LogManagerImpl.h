#ifndef _LOG_MANAGER_IMPL_H
#define _LOG_MANAGER_IMPL_H

#include <string>

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
		LogManagerImpl();
		~LogManagerImpl();

		bool Init();
		#ifdef ANDROID
			bool Init(bool LogToFile, bool LogToStdOut, std::string absFilePath, JNIEnv* env);
		#else
			bool Init(bool LogToFile, bool LogToStdOut, std::string absFilePath);
		#endif
		bool Destroy();
		bool Start();
		bool Stop();
		void Tick();
		bool isRunning();

		void AddStream(std::streambuf* sb);
		void RemoveStream(std::streambuf* sb);
		bool IsStreamAdded(std::streambuf* sb);
		std::string GetFileLogPath();

		std::ostream& LogInfo();
		std::ostream& LogDebug();
		std::ostream& LogWarn();
		std::ostream& LogError();

	private:
		std::string InternalGetLogName();
		std::string InternalGetLogPath();

		bool mRunning;
		bool mInitialized;
		bool mLogToFileDefault;
		bool mLogToOSDefault;
		std::string mLogPath;
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
