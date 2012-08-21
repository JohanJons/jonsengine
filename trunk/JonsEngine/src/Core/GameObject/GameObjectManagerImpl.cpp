#include "include/Core/GameObject/GameObjectManagerImpl.h"

namespace JonsEngine
{
	IMemoryAllocator* GameObjectManagerImpl::IGameObjectManager::mMemoryAllocator = NULL;
	std::map<uint64_t, boost::weak_ptr<GameObject> > GameObjectManagerImpl::IGameObjectManager::mObjects;

	GameObjectManagerImpl::GameObjectManagerImpl(ILogManager& logger, IMemoryAllocator* memAllocator) : mRunning(false), mInitialized(false), mLog(logger)
	{
		mMemoryAllocator = memAllocator;
	}

	GameObjectManagerImpl::~GameObjectManagerImpl()
	{
		if (mInitialized)
			Destroy();
	}

	bool GameObjectManagerImpl::Init()
	{
		bool ret = true;

		mInitialized = true;
		
		return ret;
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
			mLog.LogWarn() <<  "GameObjectManagerImpl::Start(): GameObjectManager already started!" << std::endl;

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
			mLog.LogWarn() <<  "GameObjectManagerImpl::Stop(): GameObjectManager already stopped!" << std::endl;

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