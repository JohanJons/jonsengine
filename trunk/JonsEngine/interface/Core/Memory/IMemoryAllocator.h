#pragma once

#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class IMemoryAllocator
	{
	public:
		virtual ~IMemoryAllocator() { }

		/*
		 * Allocates a block of memory.
		 * @param size: memoryblock size in bytes to allocate. If NULL, no memory will be allocated.
		 * @return: address of the allocated memoryblock if successfull, otherwise NULL
		 */
		virtual void* Allocate(size_t size) = 0;

		/*
		 * Reallocates a block of memory.
		 * @param memblock: address of the memory block to reallocate. If NULL, no memory will be reallocated.
		 * @param size: memoryblock size in bytes to reallocate. If NULL, no memory will be reallocated.
		 * @return: address of the reallocated memoryblock if successfull, otherwise NULL
		 */
        virtual void* Reallocate(void* memblock, size_t size) = 0;

		/*
		 * Deallocates a block of memory.
		 * @param memblock: address of the memory block to be deallocated. If NULL, no memory will be deallocated.
		 */
        virtual void Deallocate(void* memblock) = 0;

		

		/*
		 * Allocates a given object and passes on up to 4 given arguments to its constructor.
		 * @param [...]: arguments that will be passed to the constructor of the object allocated. Can pass up to 4 arguments.
		 *				 For more, allocate memory and call object constructor manually
		 * @return: address of the constructed object if successfull, otherwise NULL.
		 */
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


		/*
		 * Calls the destructor and deallocates memory of a object
		 * @param obj: pointer to the object to deallocate. If NULL, does nothing.
		 */
		template <class T>
		inline void DeallocateObject(T* obj)
		{
			obj->~T();
			InternalDeallocate(obj);
		}

		/*
		 * Returns the total amount of allocated memory in bytes
		 * @return: The total amount of memory allocated.
		 */
		virtual uint64_t GetAllocatedMemory() const = 0;

		/*
		 * Returns the name of the memory allocator.
		 * @return: name of the allocator.
		 */
		virtual const std::string& GetAllocatorName() const = 0;




	private:
		virtual void* InternalAllocate(size_t size) = 0;
		virtual void InternalDeallocate(void* p) = 0;
	};


	namespace Globals
	{
		/*
		 * Returns the default engine heap allocator.
		 * @return: The default heap allocator for the engine.
		 */

		IMemoryAllocator& GetDefaultHeapAllocator();
	}

}