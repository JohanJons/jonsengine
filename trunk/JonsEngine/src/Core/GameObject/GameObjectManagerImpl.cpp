#include "include/Core/GameObject/GameObjectManagerImpl.h"

namespace JonsEngine
{
	IMemoryManager* GameObjectManagerImpl::IGameObjectManager::mMemoryMgr = NULL;
	std::map<uint64_t, boost::weak_ptr<GameObject> > GameObjectManagerImpl::IGameObjectManager::mObjects;

	GameObjectManagerImpl::GameObjectManagerImpl() : mRunning(false), mInitialized(false), mLog(NULL)
	{
		mMemoryMgr = NULL;
	}

	GameObjectManagerImpl::~GameObjectManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool GameObjectManagerImpl::Init(ILogManager* const logger, IMemoryManager* const memmgr)
	{
		mLog = logger;
		mMemoryMgr = memmgr;

		if (mLog && mMemoryMgr)
		{
			mInitialized = true;
			return true;
		}
		else
			return false;
	}

	bool GameObjectManagerImpl::Destroy()
	{
		if (mInitialized)
		{
			bool res = true;

			if (mRunning)
				res &= Stop();

			mInitialized = false;

			return res;
		}
		else
			return false;
	}

	bool GameObjectManagerImpl::Start()
	{
		bool res = true;

		if (!mRunning)
		{
			mRunning = true;

		}
		else
			mLog->LogWarn() <<  "GameObjectManagerImpl::Start(): GameObjectManager already started!" << std::endl;

		return res;
	}

	bool GameObjectManagerImpl::Stop()
	{
		bool res = true;

		if (mRunning)
		{
			mRunning = false;

		}
		else
			mLog->LogWarn() <<  "GameObjectManagerImpl::Stop(): GameObjectManager already stopped!" << std::endl;

		return res;
	}

	bool GameObjectManagerImpl::isRunning()
	{
		return mRunning;
	}

	void GameObjectManagerImpl::Tick()
	{

	}

}