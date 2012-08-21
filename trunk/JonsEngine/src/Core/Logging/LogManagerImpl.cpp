#include "include/Core/Logging/LogManagerImpl.h"

namespace JonsEngine
{
	LogManagerImpl::LogManagerImpl() : mRunning(false), mLogToFileDefault(false), mLogToOSDefault(false), 
										mStreamBuf(NULL), mFileStream(NULL), mDummyStreamBuf(NULL), mMemoryAllocator(NULL)
	{
		mLogStream = NULL;

		#ifdef ANDROID
			mAndroidLogBuf = NULL;
		#endif
	}

	LogManagerImpl::~LogManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool LogManagerImpl::Init(IMemoryAllocator* const memAllocator)
	{
		return Init(false, false, "", memAllocator);
	}

	#ifdef ANDROID
	bool LogManagerImpl::Init(bool LogToFileDefault, bool LogToOSDefault, const std::string& absFilePath, IMemoryAllocator* const memAllocator, JNIEnv* jenv)
	{
		mJNIEnv = jenv;

		return Init(LogToFileDefault, LogToOSDefault, absFilePath, memAllocator);
	}
	#endif

	bool LogManagerImpl::Init(bool LogToFileDefault, bool LogToOSDefault, const std::string& absFilePath, IMemoryAllocator* const memAllocator)
	{
		if (memAllocator)
		{
			std::string LogPath;
			mLogToFileDefault = LogToFileDefault;
			mLogToOSDefault = LogToOSDefault;
			mMemoryAllocator = memAllocator;

			mStreamBuf = mMemoryAllocator->AllocateObject<JonsStreamBuf>();

			if (mStreamBuf)
			{
				if (mLogToFileDefault)
				{
					if (absFilePath.empty())
					{
						LogPath += InternalGetLogPath(); 
						LogPath += InternalGetLogName();
					}
					else
						LogPath += absFilePath;

					mLogPath = LogPath;
					mFileStream = mMemoryAllocator->AllocateObject<std::ofstream>(LogPath.c_str(), std::ios::trunc);
				}

				#ifdef ANDROID
					if (mLogToOSDefault && !mAndroidLogBuf)
						mAndroidLogBuf = mMemoryAllocator->AllocateObject<AndroidLogStreamBuf>();
				#endif
		
				if (!mLogToOSDefault && !mLogToFileDefault)
				{
					mDummyStreamBuf = mMemoryAllocator->AllocateObject<DummyLogStreamBuf>();
				}

				mLogStream = mMemoryAllocator->AllocateObject<JonsOutputStream>(mStreamBuf); 
			}
		
			if (mStreamBuf && mLogStream)
			{
				mInitialized = true;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	bool LogManagerImpl::Destroy()
	{
		if (mInitialized)
		{
			bool res = true;

			if (mRunning)
				res &= Stop();

			if (mFileStream)
			{
				mFileStream->close();
				mMemoryAllocator->DeallocateObject(mFileStream);
				mFileStream = NULL;
			}

			if (mStreamBuf)
			{
				mMemoryAllocator->DeallocateObject(mStreamBuf);
				mStreamBuf = NULL;
			}

			if (mDummyStreamBuf)
			{
				mMemoryAllocator->DeallocateObject(mDummyStreamBuf);
				mDummyStreamBuf = NULL;
			}

			#ifdef ANDROID
				if (mAndroidLogBuf)
				{
					mMemoryAllocator->DeallocateObject(mAndroidLogBuf);
					mAndroidLogBuf = NULL;
				}
			#endif
		
			if (mLogStream)
			{
				mMemoryAllocator->DeallocateObject(mLogStream);
				mLogStream = NULL;
			}

			mInitialized = false;

			return (!mRunning && !mLogStream && !mDummyStreamBuf && !mStreamBuf && !mFileStream);
		}
		else
			return false;
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


	void LogManagerImpl::AddStream(std::streambuf* const sb)
	{
		if (sb)
			mStreamBuf->AddStream(sb);
	}

	void LogManagerImpl::RemoveStream(std::streambuf* const sb)
	{
		if (sb)
			mStreamBuf->RemoveStream(sb);
	}

	bool LogManagerImpl::IsStreamAdded(std::streambuf* const sb) const
	{
		if (sb)
			return mStreamBuf->IsStreamAdded(sb);
		else
			return false;
	}

	const std::string& LogManagerImpl::GetFileLogPath() const
	{
		return mLogPath;
	}

	const std::string LogManagerImpl::InternalGetLogPath() const
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

	const std::string LogManagerImpl::InternalGetLogName() const
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
