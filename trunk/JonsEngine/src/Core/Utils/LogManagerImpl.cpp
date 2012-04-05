#include "LogManagerImpl.h"

namespace JonsEngine
{
	#ifdef ANDROID
		LogManagerImpl::LogManagerImpl(JNIEnv* env)
		{
			BaseConstructor();
		
			mAndroidLogBuf = new AndroidLogStreamBuf();
			mAndroidLogStream = new AndroidLogOStream(mAndroidLogBuf);
			mJNIEnv = env;

		}
	#else
		LogManagerImpl::LogManagerImpl()
		{
			BaseConstructor();
		}
	#endif

	void LogManagerImpl::BaseConstructor()
	{
		mRunning = false;
		mLogToFile = false;
		mLogToStdOut = false;
		
	}

	LogManagerImpl::~LogManagerImpl()
	{
		Destroy();
	}

	int LogManagerImpl::Init()
	{
		return Init(true, false);
	}

	int LogManagerImpl::Init(bool LogToFile, bool LogToStdOut)
	{
		std::string LogPath;
		mLogToFile = LogToFile;
		mLogToStdOut = LogToStdOut;

		if (mLogToFile)
		{
			LogPath += GetLogPath(); 
			LogPath += GetLogName();

			mFileStream = new std::ofstream(LogPath.c_str(), std::ios::trunc);
		}

		if (mLogToFile && mLogToStdOut)
			#ifdef ANDROID
				mStreamBuf = new JonsStreamBuf(mFileStream->rdbuf(),mAndroidLogBuf);
			#else
				mStreamBuf = new JonsStreamBuf(mFileStream->rdbuf(),std::cout.rdbuf());
			#endif
		else if (mLogToFile)
			mStreamBuf = new JonsStreamBuf(mFileStream->rdbuf());
		else 
			#ifdef ANDROID
				mStreamBuf = new JonsStreamBuf(mAndroidLogBuf);
			#else
				mStreamBuf = new JonsStreamBuf(std::cout.rdbuf());
			#endif

		mLogStream = new JonsOutputStream(mStreamBuf);


		return INIT_OK;
	}

	void LogManagerImpl::Destroy()
	{
		if (mFileStream)
			mFileStream->close();
	}

	void LogManagerImpl::Start()
	{


		mRunning = true;
	}

	void LogManagerImpl::Stop()
	{
		mRunning = false;
	}

	void LogManagerImpl::Tick()
	{

	}

	bool LogManagerImpl::isRunning()
	{
		return mRunning;
	}

	std::string LogManagerImpl::GetLogPath()
	{
		std::string ret;

		#ifdef ANDROID
			jclass cls_Env = mJNIEnv->FindClass("android/os/Environment");
			jmethodID mid_getExtStorage = mJNIEnv->GetStaticMethodID(cls_Env, "getExternalStorageDirectory",  "()Ljava/io/File;");
			jobject obj_File = mJNIEnv->CallStaticObjectMethod(cls_Env, mid_getExtStorage);

			jclass cls_File = mJNIEnv->FindClass("java/io/File");
			jmethodID mid_getPath = mJNIEnv->GetMethodID(cls_File, "getPath", "()Ljava/lang/String;");
			jstring obj_Path = (jstring) mJNIEnv->CallObjectMethod(obj_File, mid_getPath);
			const char* path = mJNIEnv->GetStringUTFChars(obj_Path, NULL);

			ret += path;
			ret += '/';

			mJNIEnv->ReleaseStringUTFChars(obj_Path,path);
		#endif

			return ret;
	}

	std::string LogManagerImpl::GetLogName()
	{
		std::stringstream ret;

		ret << __ENGINE_LOG_TAG;
		ret << "_"; ret << GetHour();
		ret << "_"; ret << GetMinute();
		ret << "_";
		ret << "_"; ret << GetDay();
		ret << "_"; ret << GetMonth();
		ret << "_"; ret << GetYear();
		ret << ".txt";

		return ret.str();
	}
	
	std::ostream& LogManagerImpl::LogInfo()
	{
		*mLogStream << __ENGINE_LOG_TAG << "_INFO: ";

		return (*mLogStream);
	}

	std::ostream& LogManagerImpl::LogDebug()
	{
		*mLogStream << __ENGINE_LOG_TAG << "_DEBUG: ";

		return (*mLogStream);
	}

	std::ostream& LogManagerImpl::LogWarn()
	{
		*mLogStream << __ENGINE_LOG_TAG << "_WARN: ";

		return (*mLogStream);
	}

	std::ostream& LogManagerImpl::LogError()
	{
		*mLogStream << __ENGINE_LOG_TAG << "_ERROR: ";

		return (*mLogStream);
	}

}
