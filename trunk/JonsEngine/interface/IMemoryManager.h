#ifndef _I_MEMORY_MANAGER_H
#define _I_MEMORY_MANAGER_H

#include "EngineDefs.h"

namespace JonsEngine
{
	class ILogManager;
	class Allocator;

	class IMemoryManager
	{
	public:
		virtual ~IMemoryManager() { }

		virtual bool Init(ILogManager* logger) = 0;
		virtual bool Init(Allocator_BackEnd allocatorBackEnd, ILogManager* logger) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		template <class T>
		inline T* AllocateObject(Allocation_Mode mode = USER) { return new (Allocate(sizeof(T),mode)) T(); }
		template <class T, class arg0>
		inline T* AllocateObject(arg0 a0, Allocation_Mode mode = USER) { return new (Allocate(sizeof(T),mode)) T(a0); }
		template <class T, class arg0, class arg1>
		inline T* AllocateObject(arg0 a0, arg1 a1, Allocation_Mode mode = USER) { return new (Allocate(sizeof(T),mode)) T(a0,a1); }
		template <class T, class arg0, class arg1, class arg2>
		inline T* AllocateObject(arg0 a0, arg1 a1, arg2 a2, Allocation_Mode mode = USER) { return new (Allocate(sizeof(T),mode)) T(a0,a1,a2); }

		template <class T>
		inline void DeAllocateObject(T* obj, size_t size)
		{
			obj->~T();
			DeAllocate(obj,size);
		}

		virtual void* Allocate(size_t size, Allocation_Mode mode = USER) = 0;
		virtual void* ReAllocate(void* p, size_t n) = 0;
		virtual void DeAllocate(void* obj, size_t size, Allocation_Mode mode = USER) = 0;

		virtual Allocator* GetAllocator() = 0;
		virtual uint64_t GetTotalAllocatedMemory() = 0;
		virtual uint64_t GetUserAllocatedMemory() = 0;
		virtual uint64_t GetInternalAllocatedMemory() = 0;
	};

}

#endif
