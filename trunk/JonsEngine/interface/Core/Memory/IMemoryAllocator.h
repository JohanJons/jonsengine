#ifndef _I_MEMORY_ALLOCATOR_H
#define _I_MEMORY_ALLOCATOR_H

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
	class IMemoryAllocator
	{
	public:
		virtual ~IMemoryAllocator() { }

		virtual void* Allocate(size_t size) = 0;
        virtual void* Reallocate(void* p, size_t size) = 0;
        virtual void Deallocate(void* p) = 0;

		template <class T>
		inline T* AllocateObject() { return new (InternalAllocate(sizeof(T))) T(); }

		template <class T, class arg0>
		inline T* AllocateObject(arg0 a0) { return new (InternalAllocate(sizeof(T))) T(a0); }

		template <class T, class arg0, class arg1>
		inline T* AllocateObject(arg0 a0, arg1 a1) { return new (InternalAllocate(sizeof(T))) T(a0,a1); }

		template <class T, class arg0, class arg1, class arg2>
		inline T* AllocateObject(arg0 a0, arg1 a1, arg2 a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, class arg0, class arg1, class arg2, class arg3>
		inline T* AllocateObject(arg0 a0, arg1 a1, arg2 a2, arg3 a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T>
		inline void DeallocateObject(T* obj)
		{
			obj->~T();
			InternalDeallocate(obj);
		}

		virtual uint64_t GetAllocatedMemory() const = 0;

	private:
		virtual void* InternalAllocate(size_t size) = 0;
		virtual void InternalDeallocate(void* p) = 0;
	};

}

#endif
