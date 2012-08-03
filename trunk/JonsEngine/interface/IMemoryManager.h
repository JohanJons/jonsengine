#ifndef _I_MEMORY_MANAGER_H
#define _I_MEMORY_MANAGER_H

#include "EngineDefs.h"

namespace JonsEngine
{
	class ILogManager;
	class HeapAllocator;

	class IMemoryManager
	{
	public:
		virtual ~IMemoryManager() { }

		virtual bool Init(ILogManager* const logger) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		template <class T>
		inline T* AllocateObject() { return new (InternalAllocate(sizeof(T))) T(); }
		template <class T, class arg0>
		inline T* AllocateObject(arg0 a0) { return new (InternalAllocate(sizeof(T))) T(a0); }
		template <class T, class arg0, class arg1>
		inline T* AllocateObject(arg0 a0, arg1 a1) { return new (InternalAllocate(sizeof(T))) T(a0,a1); }
		template <class T, class arg0, class arg1, class arg2>
		inline T* AllocateObject(arg0 a0, arg1 a1, arg2 a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }
		template <class T>
		inline void DeAllocateObject(T* obj)
		{
			obj->~T();
			InternalDeAllocate(obj);
		}

		virtual HeapAllocator* const GetHeapAllocator() = 0;
		virtual uint64_t GetTotalAllocatedMemory() = 0;

	private:
		virtual void* InternalAllocate(size_t size) = 0;
		virtual void InternalDeAllocate(void* p) = 0;
	};

}

#endif
