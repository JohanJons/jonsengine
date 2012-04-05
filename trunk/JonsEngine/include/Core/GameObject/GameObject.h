#ifndef _JONS_GAMEOBJECT_H
#define _JONS_GAMEOBJECT_H

#include "../../../Interface/EngineDefs.h"
#include "../../../Interface/IMemoryManager.h"

namespace JonsEngine
{
	class GameObject
	{
	friend class IGameObjectManager;

	public:
		virtual ~GameObject() { }

	private:
		uint64_t GameObjectID;

	};

}

#endif