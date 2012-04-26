#ifndef _JONS_VECTOR_H
#define _JONS_VECTOR_H


#include "../Memory/HeapAllocator.h"

namespace JonsEngine
{

	template <class T>
	class Vector
	{
	public:
		const size_t DefaultCapacityIncrease;
		typedef T* Iterator; 

		Vector(HeapAllocator* allocator);
		Vector(HeapAllocator* allocator, size_t size);
		~Vector();

		Iterator begin();
		Iterator end();

		T& front();
		T& back();
		T& at(size_t n);
		T& operator[](size_t n);

		size_t capacity();
		size_t size();
		bool empty();
		void clear();
		void reset();

		Iterator insert(Iterator position, const T& value);
		void insert(Iterator position,size_t n, const T& value);
		void insert(Iterator position,Iterator first, Iterator last);
		void erase(Iterator position);
		void erase(Iterator first, Iterator last);
		void resize(size_t numElements);

		void push_back(const T& item);
		void pop_back();

	private:
		void CopyValues(Iterator pBegin, Iterator pEnd, Iterator pTarget);
		size_t GetNewCapacity(size_t currentCapacity);
		size_t GetNewCapacity(size_t currentCapacity, size_t minimumIncrease);

		T* mBegin;
		T* mEnd;
		T* mCapacity;
		HeapAllocator* mAllocator;

	};

	template <class T>
	inline Vector<T>::Vector(HeapAllocator* allocator) : mAllocator(allocator), mCapacity(NULL), mBegin(NULL), mEnd(NULL), DefaultCapacityIncrease(2)
	{
	}

	template <class T>
	inline Vector<T>::Vector(HeapAllocator* allocator, size_t size) : mAllocator(allocator), DefaultCapacityIncrease(2)
	{
		mBegin = (T*) mAllocator->Allocate(sizeof(T)*size);
		mEnd = mBegin;
		mCapacity = mBegin + size;
	}

	template <class T>
	inline Vector<T>::~Vector()
	{
		if (mBegin)
		{
			clear();

			mAllocator->DeAllocate(mBegin);
		}
	}

	template <class T>
	inline typename Vector<T>::Iterator
	Vector<T>::begin()
	{
		return mBegin;
	}
	
	template <class T>
	inline typename Vector<T>::Iterator
	Vector<T>::end()
	{
		return mEnd;
	}

	template <class T>
	inline T& Vector<T>::front()
	{
		return *mBegin;
	}
	
	template <class T>
	inline T& Vector<T>::back()
	{
		return *mEnd;
	}
	
	template <class T>
	inline T& Vector<T>::at(size_t n)
	{
		return *(mBegin+n);
	}

	template <class T>
	inline T& Vector<T>::operator[](size_t n)
	{
		return *(mBegin+n);
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
	inline void Vector<T>::reset()
	{
		if (mBegin)
		{
			clear();

			mAllocator->DeAllocate(mBegin);

			mCapacity = mEnd = mBegin = NULL;
		}
	}

	template <class T>
	inline typename Vector<T>::Iterator 
	Vector<T>::insert(Iterator position, const T& value)
	{
		const size_t pos = (position - mBegin);

		// no need to reallocate
		if (mEnd != mCapacity)
		{
			memcpy(mBegin+pos+1,mBegin+pos,sizeof(T)*(mEnd-position));
			memcpy(mBegin+pos,&value,sizeof(T));

			mEnd++;

			return mBegin+pos;
		}
		else
		{
			const size_t prevSize = mEnd - mBegin;
			const size_t newSize = GetNewCapacity(prevSize);
			T* newBegin = (T*) mAllocator->Allocate(newSize*sizeof(T));

			memcpy(newBegin,mBegin,sizeof(T)*pos);
			memcpy(newBegin+pos,&value,sizeof(T));
			memcpy(newBegin+pos+1,mBegin+pos,(mEnd - position)*sizeof(T));
			
			mEnd = newBegin + (mEnd - mBegin) + 1;
			mAllocator->DeAllocate(mBegin);
			mBegin = newBegin;
			mCapacity = mBegin+newSize;

			return mBegin+pos;
		}
	}
	
	template <class T>
	inline void Vector<T>::insert(Iterator position, size_t n, const T& value)
	{
		const size_t pos = (position - mBegin);

		// no need to reallocate
		if (n <= uint32_t(mCapacity-mEnd))
		{
			memcpy(mBegin+pos+n,mBegin+pos,sizeof(T)*(mEnd-position));

			for (uint32_t i = 0; i<n; i++)
				memcpy(mBegin+pos+i,&value,sizeof(T));

			mEnd += n;
		}
		else
		{
			// TODO MERGE
			const size_t prevSize = mEnd - mBegin;
			const size_t newSize = GetNewCapacity(prevSize,n);
			T* newBegin = (T*) mAllocator->Allocate(newSize*sizeof(T));

			memcpy(newBegin,mBegin,sizeof(T)*pos);
			memcpy(newBegin+pos,&value,sizeof(T)*n);
			memcpy(newBegin+pos+n,mBegin+pos,(mEnd - position)*sizeof(T));

			mEnd = newBegin + (mEnd - mBegin) + n;
			mAllocator->DeAllocate(mBegin);
			mBegin = newBegin;
			mCapacity = mBegin+newSize;
		}
	}
	
	template <class T>
	inline void Vector<T>::insert(Iterator position,Iterator first, Iterator last)
	{

	}

	template <class T>
	inline typename Vector<T>::Iterator 
	Vector<T>::erase(Iterator position)
	{

	}

	template <class T>
	inline typename Vector<T>::Iterator 
	Vector<T>::erase(Iterator position)
	{

	}

	template <class T>
	inline void Vector<T>::resize(size_t numElements)
	{
		const size_t prevSize = mEnd - mBegin;

		T* newBegin = (T*) mAllocator->Allocate(numElements*sizeof(T));
		memcpy(newBegin,mBegin,numElements*sizeof(T));
		mAllocator->DeAllocate(mBegin);

		mBegin = newBegin;
		mEnd = mBegin + (numElements > prevSize ? prevSize : numElements);
		mCapacity = mBegin + numElements;
	}

	template <class T>
	inline void Vector<T>::push_back(const T& item)
	{
		if (mEnd != mCapacity)
		{
			mEnd++;
			memcpy(mEnd,&item,sizeof(T));
		}
		else
			insert(mEnd,item);
	}

	template <class T>
	inline void Vector<T>::pop_back()
	{
		mEnd->~T();
		mEnd--;
	}



	/*
	 * Private
	 */
	template <class T>
	inline size_t Vector<T>::GetNewCapacity(size_t currentCapacity)
	{
		return (currentCapacity > 0) ? (DefaultCapacityIncrease*currentCapacity) : DefaultCapacityIncrease;
	}
	template <class T>
	inline size_t Vector<T>::GetNewCapacity(size_t currentCapacity, size_t minimum)
	{
		return (currentCapacity >= 0 && currentCapacity <= minimum) ? (minimum+DefaultCapacityIncrease) : currentCapacity*DefaultCapacityIncrease;
	}

	/*template <class T>
	inline void Vector<T>::CopyValues(Iterator pBegin, Iterator pEnd, Iterator pTarget):
	{
		for (Iterator iter = pBegin; iter != pEnd; iter++)
			memcpy(pBegin,pTarget
	}*/

}

#endif