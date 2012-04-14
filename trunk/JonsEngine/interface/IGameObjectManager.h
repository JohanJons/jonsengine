#ifndef _I_GAMEOBJECT_MANAGER_H
#define _I_GAMEOBJECT_MANAGER_H

#include "../../Thirdparty/boost_1_49_0/boost/smart_ptr/shared_ptr.hpp"
#include "../../Thirdparty/boost_1_49_0/boost/smart_ptr/weak_ptr.hpp"

#include "EngineDefs.h"
#include "IMemoryManager.h"
#include "ILogManager.h"
#include "../Include/Core/GameObject/GameObject.h"

namespace JonsEngine
{
	
	class IGameObjectManager
	{
	public:
		IGameObjectManager() : mNextGameObjID(1)  { }
		virtual ~IGameObjectManager() { mObjects.clear(); }

		virtual bool Init(ILogManager* logger, IMemoryManager* memmgr) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		template <class T>
		static void DestroyObject(T* obj)
		{
			GameObject* ob = static_cast<GameObject*>(obj);
			mObjects.erase(ob->GameObjectID);
		
			mMemoryMgr->DeAllocateObject(obj);
		}

		
		template <class T>
		boost::shared_ptr<T> CreateObject()
		{
			boost::shared_ptr<T> sPtr(mMemoryMgr->AllocateObject<T>(),DestroyObject<T> );
			boost::weak_ptr<GameObject> wPtr(sPtr);

			wPtr.lock()->GameObjectID = mNextGameObjID;
			
			mObjects.insert(std::pair<uint64_t,boost::weak_ptr<GameObject>>(mNextGameObjID,wPtr));

			mNextGameObjID++;
			return sPtr;
		}

	protected:
		static IMemoryManager* mMemoryMgr;
		static std::map<uint64_t, boost::weak_ptr<GameObject>> mObjects;
		uint64_t mNextGameObjID;
	};

}

#endif