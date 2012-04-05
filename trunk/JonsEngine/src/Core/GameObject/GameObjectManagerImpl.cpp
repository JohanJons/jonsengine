#include "../../../Include/Core/GameObject/GameObjectManagerImpl.h"

namespace JonsEngine
{
	IMemoryManager* GameObjectManagerImpl::IGameObjectManager::mMemoryMgr = NULL;
	std::map<uint64_t, boost::weak_ptr<GameObject>> GameObjectManagerImpl::IGameObjectManager::mObjects;

	GameObjectManagerImpl::GameObjectManagerImpl(ILogManager* logger, IMemoryManager* memmgr) : 
										mRunning(false), mLog(logger)
	{
		mMemoryMgr = memmgr;
	}

	GameObjectManagerImpl::~GameObjectManagerImpl()
	{
		Destroy();
	}

	int32_t GameObjectManagerImpl::Init()
	{

		if (mLog)
			return INIT_OK;
		else
			return INIT_NOK;
	}

	bool GameObjectManagerImpl::Destroy()
	{
		bool res = true;

		if (mRunning)
			res &= Stop();

		

		return res;
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