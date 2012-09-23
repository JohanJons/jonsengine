#pragma once

#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class IMemoryAllocator
	{
	public:
		virtual ~IMemoryAllocator() { }

		virtual void* Allocate(size_t size) = 0;
        virtual void* Reallocate(void* p, size_t size) = 0;
        virtual void Deallocate(void* p) = 0;

		// 0 - 4 params can be supplied to AllocateObject... for more, manually allocate and call constructor

		// 0 args to constructor
		template <class T>
		inline T* AllocateObject() { return new (InternalAllocate(sizeof(T))) T(); }

		// 1 args to constructor
		template <class T, typename arg0>
		inline T* AllocateObject(const arg0& a0) { return new (InternalAllocate(sizeof(T))) T(a0); }

		template <class T, typename arg0>
		inline T* AllocateObject(arg0& a0) { return new (InternalAllocate(sizeof(T))) T(a0); }

		// 2 args to constructor
		template <class T, typename arg0, typename arg1>
		inline T* AllocateObject(arg0& a0, arg1& a1) { return new (InternalAllocate(sizeof(T))) T(a0,a1); }

		template <class T, typename arg0, typename arg1>
		inline T* AllocateObject(const arg0& a0, arg1& a1) { return new (InternalAllocate(sizeof(T))) T(a0,a1); }

		template <class T, typename arg0, typename arg1>
		inline T* AllocateObject(arg0& a0, const arg1& a1) { return new (InternalAllocate(sizeof(T))) T(a0,a1); }

		template <class T, typename arg0, typename arg1>
		inline T* AllocateObject(const arg0& a0, const arg1& a1) { return new (InternalAllocate(sizeof(T))) T(a0,a1); }

		// 3 args to constructor
		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(arg0& a0, arg1& a1, arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(const arg0& a0, arg1& a1, arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(arg0& a0, const arg1& a1, arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(arg0& a0, arg1& a1, const arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(const arg0& a0, const arg1& a1, arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(const arg0& a0, arg1& a1, const arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(arg0& a0, const arg1& a1, const arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		template <class T, typename arg0, typename arg1, typename arg2>
		inline T* AllocateObject(const arg0& a0, const arg1& a1, const arg2& a2) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2); }

		// 4 args to constructor
		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, arg1& a1, arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, arg1& a1, arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, const arg1& a1, arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, arg1& a1, const arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, arg1& a1, arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, const arg1& a1, arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, arg1& a1, const arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, arg1& a1, arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, const arg1& a1, const arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, const arg1& a1, arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, arg1& a1, const arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, const arg1& a1, const arg2& a2, arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, const arg1& a1, arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, arg1& a1, const arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(arg0& a0, const arg1& a1, const arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T, typename arg0, typename arg1, typename arg2, typename arg3>
		inline T* AllocateObject(const arg0& a0, const arg1& a1, const arg2& a2, const arg3& a3) { return new (InternalAllocate(sizeof(T))) T(a0,a1,a2,a3); }

		template <class T>
		inline void DeallocateObject(T* obj)
		{
			obj->~T();
			InternalDeallocate(obj);
		}

		virtual uint64_t GetAllocatedMemory() const = 0;
		virtual const std::string& GetAllocatorName() const = 0;

	private:
		virtual void* InternalAllocate(size_t size) = 0;
		virtual void InternalDeallocate(void* p) = 0;
	};

}