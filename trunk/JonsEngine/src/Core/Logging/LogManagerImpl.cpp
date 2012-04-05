#include "../../../Include/Core/Logging/LogManagerImpl.h"

namespace JonsEngine
{
	#ifdef ANDROID
		LogManagerImpl::LogManagerImpl(JNIEnv* env) : mAndroidLogBuf(NULL), mJNIEnv(env)
		{
			BaseConstructor();

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
		mLogToFileDefault = false;
		mLogToOSDefault = false;
		
		mStreamBuf = NULL;
		mFileStream = NULL;
		mLogStream = NULL;
		mDummyStreamBuf = NULL;
	}

	LogManagerImpl::~LogManagerImpl()
	{
		Destroy();
	}

	int32_t LogManagerImpl::Init()
	{
		return Init(true, false);
	}

	int32_t LogManagerImpl::Init(bool LogToFileDefault, bool LogToOSDefault)
	{
		std::string LogPath;
		mLogToFileDefault = LogToFileDefault;
		mLogToOSDefault = LogToOSDefault;

		mStreamBuf = new JonsStreamBuf();

		if (mStreamBuf)
		{
			if (mLogToFileDefault)
			{
				LogPath += GetLogPath(); 
				LogPath += GetLogName();

				mFileStream = new std::ofstream(LogPath.c_str(), std::ios::trunc);
			}

			#ifdef ANDROID
				if (mLogToOSDefault && !mAndroidLogBuf)
					mAndroidLogBuf = new AndroidLogStreamBuf();
			#endif
		
			if (!mLogToOSDefault && !mLogToFileDefault)
			{
				mDummyStreamBuf = new DummyLogStreamBuf();
			}

			mLogStream = new JonsOutputStream(mStreamBuf);
		}
		
		if (!mStreamBuf || !mLogStream)
			return INIT_NOK;
		else
			return INIT_OK;
	}

	bool LogManagerImpl::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		if (mFileStream)
		{
			mFileStream->close();
			delete mFileStream;
			mFileStream = NULL;
		}

		if (mStreamBuf)
		{
			delete mStreamBuf;
			mStreamBuf = NULL;
		}

		if (mDummyStreamBuf)
		{
			delete mDummyStreamBuf;
			mDummyStreamBuf = NULL;
		}

		#ifdef ANDROID
			if (mAndroidLogBuf)
			{
				delete mAndroidLogBuf;
				mAndroidLogBuf = NULL;
			}
		#endif
		
		if (mLogStream)
		{
			delete mLogStream;
			mLogStream = NULL;
		}

		return (!mRunning && !mLogStream && !mDummyStreamBuf && !mStreamBuf && !mFileStream);
	}

	bool LogManagerImpl::Start()
	{
		bool res = true;

		if (!mRunning)
		{
			mRunning = true;

			/* Default File logging */
			if (mFileStream && mLogToFileDefault)
			{
				AddStream(mFileStream->rdbuf());
				res &= IsStreamAdded(mFileStream->rdbuf());
			}

			/* Default OS logging */
			#ifdef ANDROID
				if (mAndroidLogBuf && mLogToOSDefault)
				{
					AddStream(mAndroidLogBuf);
					res &= IsStreamAdded(mAndroidLogBuf);
				}
			#else
				if (mLogToOSDefault)
				{
					AddStream(std::cout.rdbuf());
					res &= IsStreamAdded(std::cout.rdbuf());
				}
			#endif

			/* Dummy logger */
			if (mDummyStreamBuf && !mLogToOSDefault && !mFileStream)
			{
				AddStream(mDummyStreamBuf);
				res &= IsStreamAdded(mDummyStreamBuf);
			}

		}

		return res;
	}

	bool LogManagerImpl::Stop()
	{
		bool res = true;

		if (mRunning)
		{
			mRunning = false;

			/* Default File logging */
			if (mFileStream && mLogToFileDefault && IsStreamAdded(mFileStream->rdbuf()))
			{
				RemoveStream(mFileStream->rdbuf());
				res &= !IsStreamAdded(mFileStream->rdbuf());
			}

			/* Default OS logging */
			#ifdef ANDROID
				if (mAndroidLogBuf && mLogToOSDefault && IsStreamAdded(mAndroidLogBuf))
				{
					RemoveStream(mAndroidLogBuf);
					res &= !IsStreamAdded(mAndroidLogBuf);
				}
			#else
				if (mLogToOSDefault && IsStreamAdded(std::cout.rdbuf()))
				{
					RemoveStream(std::cout.rdbuf());
					res &= !IsStreamAdded(std::cout.rdbuf());
				}
			#endif

			/* Dummy logger */
			if (mDummyStreamBuf && !mLogToOSDefault && !mFileStream && IsStreamAdded(mDummyStreamBuf))
			{
				RemoveStream(mDummyStreamBuf);
				res &= !IsStreamAdded(mDummyStreamBuf);
			}
		}


		return res;
	}

	void LogManagerImpl::Tick()
	{

	}

	bool LogManagerImpl::isRunning()
	{
		return mRunning;
	}


	void LogManagerImpl::AddStream(std::streambuf* sb)
	{
		if (sb)
			mStreamBuf->AddStream(sb);
	}

	void LogManagerImpl::RemoveStream(std::streambuf* sb)
	{
		if (sb)
			mStreamBuf->RemoveStream(sb);
	}

	bool LogManagerImpl::IsStreamAdded(std::streambuf* sb)
	{
		if (sb)
			return mStreamBuf->IsStreamAdded(sb);
		else
			return false;
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

		ret << EngineLogTag;
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
		*mLogStream << EngineLogTag << "_INFO: ";

		return (*mLogStream);
	}

	std::ostream& LogManagerImpl::LogDebug()
	{
		*mLogStream << EngineLogTag << "_DEBUG: ";

		return (*mLogStream);
	}

	std::ostream& LogManagerImpl::LogWarn()
	{
		*mLogStream << EngineLogTag << "_WARN: ";

		return (*mLogStream);
	}

	std::ostream& LogManagerImpl::LogError()
	{
		*mLogStream << EngineLogTag << "_ERROR: ";

		return (*mLogStream);
	}

}
