#ifndef _LOG_MANAGER_IMPL_H
#define _LOG_MANAGER_IMPL_H

#include <string>

#ifdef ANDROID
#include "jni.h"
#include "AndroidLogStream.h"
#endif

#include "../../../Interface/ILogManager.h"
#include "../../../Interface/EngineDefs.h"
#include "../../../interface/IMemoryManager.h"

#include "../Utils/JonsTime.h"
#include "JonsStreamBuf.h"
#include "JonsOutputStream.h"
#include "DummyLogStream.h"


namespace JonsEngine
{
	class LogManagerImpl : public ILogManager
	{
	public:
		LogManagerImpl();
		~LogManagerImpl();

		bool Init(IMemoryManager* memmgr);
		#ifdef ANDROID
			bool Init(bool LogToFile, bool LogToStdOut, std::string absFilePath, JNIEnv* env,IMemoryManager* memmgr);
		#else
			bool Init(bool LogToFile, bool LogToStdOut, std::string absFilePath,IMemoryManager* memmgr);
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

		IMemoryManager* mMemoryManager;
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

	};

}

#endif
