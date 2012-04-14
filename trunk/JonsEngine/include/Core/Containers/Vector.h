#ifndef _JONS_VECTOR_H
#define _JONS_VECTOR_H

#include "../Memory/HeapAllocator.h"

namespace JonsEngine
{

	template <class T>
	class Vector
	{
	public:
		Vector(HeapAllocator* allocator);
		Vector(HeapAllocator* allocator, size_t size);
		~Vector();

		size_t capacity();
		size_t size();
		bool empty();
		void clear();
		void resize(size_t numElements);
		void push_back(const T& item);
		void pop_back();

	private:
		T* mBegin;
		T* mEnd;
		T* mCapacity;
		HeapAllocator* mAllocator;

	};

	template <class T>
	inline Vector<T>::Vector(HeapAllocator* allocator) : mAllocator(allocator), mCapacity(0), mBegin(NULL), mEnd(NULL)
	{
	}

	template <class T>
	inline Vector<T>::Vector(HeapAllocator* allocator, size_t n) : mAllocator(allocator)
	{
		mBegin = mAllocator->Allocate(sizeof(T)*n);
		mEnd = mBegin;
		mCapacity = mBegin + size;
	}

	template <class T>
	inline Vector<T>::~Vector()
	{
		if (mBegin)
			mAllocator->DeAllocate(mBegin);
	}

	template <class T>
	inline size_t Vector<T>::capacity()
	{
		return (size_t)(mCapacity - mBegin);
	}

	template <class T>
	inline size_t Vector<T>::size()
	{
		return (size_t)(mEnd - mBegin);
	}

	template <class T>
	inline bool Vector<T>::empty()
	{
		return (mBegin == mEnd);
	}

	template <class T>
	inline void Vector<T>::clear()
	{
		for (T* p = mBegin; p != mEnd; p++)
			p->~T();

		mEnd = mBegin;
	}

	template <class T>
	inline void Vector<T>::resize(size_t numElements)
	{

		//mBegin = mAllocator->ReAllocate(mBegin,sizeof(T)*numElements);
	}


}

#endif