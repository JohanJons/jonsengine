#ifndef _OBJECT_MANAGER_IMPL_H
#define _OBJECT_MANAGER_IMPL_H

#include <map>

#include "interface/Core/GameObject/IGameObjectManager.h"
#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Memory/IMemoryManager.h"
#include "interface/Core/EngineDefs.h"

#include "include/Core/GameObject/GameObject.h"

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