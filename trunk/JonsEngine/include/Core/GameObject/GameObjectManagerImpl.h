#ifndef _OBJECT_MANAGER_IMPL_H
#define _OBJECT_MANAGER_IMPL_H

#include <map>

#include "../../../Interface/IGameObjectManager.h"
#include "../../../Interface/ILogManager.h"
#include "../../../Interface/IMemoryManager.h"
#include "../../../Interface/EngineDefs.h"
#include "GameObject.h"

namespace JonsEngine
{
	class GameObjectManagerImpl : public IGameObjectManager
	{
	public:
		GameObjectManagerImpl();
		~GameObjectManagerImpl();

		bool Init(ILogManager* const logger, IMemoryManager* const memmgr);
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();

	private:
		ILogManager* mLog;
		bool mRunning;
		bool mInitialized;
	};


}

#endif