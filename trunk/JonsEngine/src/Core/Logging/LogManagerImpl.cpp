#include "../../../Include/Core/Logging/LogManagerImpl.h"

namespace JonsEngine
{
	LogManagerImpl::LogManagerImpl() : mRunning(false), mLogToFileDefault(false), mLogToOSDefault(false), 
										mStreamBuf(NULL), mFileStream(NULL), mDummyStreamBuf(NULL), mMemoryManager(NULL)
	{
		mLogStream = NULL;
	}

	LogManagerImpl::~LogManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool LogManagerImpl::Init(IMemoryManager* memmgr)
	{
		return Init(false, false, "",memmgr);
	}

	bool LogManagerImpl::Init(bool LogToFileDefault, bool LogToOSDefault, std::string absFilePath,IMemoryManager* memmgr)
	{
		if (memmgr)
		{
			std::string LogPath;
			mLogToFileDefault = LogToFileDefault;
			mLogToOSDefault = LogToOSDefault;
			mMemoryManager = memmgr;

			mStreamBuf = mMemoryManager->AllocateObject<JonsStreamBuf>(INTERNAL);

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
					mFileStream = mMemoryManager->AllocateObject<std::ofstream>(LogPath.c_str(), std::ios::trunc, INTERNAL);
				}

				#ifdef ANDROID
					if (mLogToOSDefault && !mAndroidLogBuf)
						mAndroidLogBuf = mMemoryManager->AllocateObject<AndroidLogStreamBuf>(INTERNAL);
				#endif
		
				if (!mLogToOSDefault && !mLogToFileDefault)
				{
					mDummyStreamBuf = mMemoryManager->AllocateObject<DummyLogStreamBuf>(INTERNAL);
				}

				mLogStream = mMemoryManager->AllocateObject<JonsOutputStream>(mStreamBuf,INTERNAL); 
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
				mMemoryManager->DeAllocateObject(mFileStream,sizeof(mFileStream));
				mFileStream = NULL;
			}

			if (mStreamBuf)
			{
				mMemoryManager->DeAllocateObject(mStreamBuf,sizeof(mStreamBuf));
				mStreamBuf = NULL;
			}

			if (mDummyStreamBuf)
			{
				mMemoryManager->DeAllocateObject(mDummyStreamBuf,sizeof(mDummyStreamBuf));
				mDummyStreamBuf = NULL;
			}

			#ifdef ANDROID
				if (mAndroidLogBuf)
				{
					mMemoryManager->DeAllocateObject(mAndroidLogBuf,sizeof(mAndroidLogBuf));
					mAndroidLogBuf = NULL;
				}
			#endif
		
			if (mLogStream)
			{
				mMemoryManager->DeAllocateObject(mLogStream,sizeof(mLogStream));
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

	std::string LogManagerImpl::GetFileLogPath()
	{
		return mLogPath;
	}

	std::string LogManagerImpl::InternalGetLogPath()
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

	std::string LogManagerImpl::InternalGetLogName()
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
