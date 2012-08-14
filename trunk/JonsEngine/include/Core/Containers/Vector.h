#ifndef _JONS_VECTOR_H
#define _JONS_VECTOR_H

#include "include/Core/Memory/HeapAllocator.h"


namespace JonsEngine
{

	template <class T>
	class Vector
	{
	public:
		const size_t DefaultCapacityIncrease;
		typedef T* iterator; 
		typedef const T* const_iterator;
		typedef T& reference;
		typedef const T& const_reference;

		Vector(HeapAllocator* allocator);
		Vector(HeapAllocator* allocator, size_t size);
		Vector(const Vector<T>& vec);
		~Vector();
		HeapAllocator* GetAllocator() const;
		Vector<T>& operator=(const Vector<T>& x);

		iterator begin();
		const_iterator begin() const;

		iterator end();
		const_iterator end() const;

		reference front();
		const_reference front() const;

		reference back();
		const_reference back() const;

		reference at(size_t n);
		const_reference at(size_t n) const;

		reference operator[](size_t n);
		const_reference operator[](size_t n) const;

		size_t capacity() const;
		size_t size() const;
		bool empty() const;
		void clear();
		void reset();

		void assign(iterator first, iterator last);
		void assign(size_t size, const T& value);
		iterator insert(iterator position, const T& value);
		void insert(iterator position,size_t n, const T& value);
		void insert(iterator position,iterator first, iterator last);
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		void resize(size_t numElements, T obj = NULL);

		void push_back(const T& item);
		void pop_back();

	private:
		void CopyValues(iterator pBegin, iterator pEnd, iterator pTarget);
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
	inline Vector<T>::Vector(const Vector<T>& vec) : DefaultCapacityIncrease(2)
	{
		const size_t vecSize = vec.size();
		const size_t vecCap = vec.capacity();

		mAllocator = vec.GetAllocator();
		mBegin = (T*) mAllocator->Allocate(vecCap * sizeof(T));

		memcpy(mBegin, vec.begin(), vecSize * sizeof(T));
		mEnd = mBegin + vecSize;
		mCapacity = mBegin + vecCap;
	}

	template <class T>
	inline Vector<T>::~Vector()
	{
		if (mBegin)
		{
			clear();

			mAllocator->Deallocate(mBegin);
		}
	}

	template <class T>
	inline Vector<T>& Vector<T>::operator=(const Vector<T>& x)
	{
		if (&x != this)
		{
			const size_t newSize = x.size();

			reset();
			mAllocator = x.GetAllocator();
			mBegin = (T*) mAllocator->Allocate(sizeof(T)*newSize);
			memcpy(mBegin,x.begin(),sizeof(T)*newSize);
			mCapacity = mEnd = mBegin + newSize;
		}
		return *this;
	}

	template <class T>
	inline HeapAllocator* Vector<T>::GetAllocator() const
	{
		return mAllocator;
	}

	template <class T>
	inline typename Vector<T>::iterator
	Vector<T>::begin()
	{
		return mBegin;
	}

	template <class T>
	inline typename Vector<T>::const_iterator
	Vector<T>::begin() const
	{
		return mBegin;
	}
	
	template <class T>
	inline typename Vector<T>::iterator
	Vector<T>::end()
	{
		return mEnd;
	}

	template <class T>
	inline typename Vector<T>::const_iterator
	Vector<T>::end() const
	{
		return mEnd;
	}

	template <class T>
	inline typename Vector<T>::reference Vector<T>::front()
	{
		return *mBegin;
	}

	template <class T>
	inline typename Vector<T>::const_reference Vector<T>::front() const
	{
		return *mBegin;
	}
	
	template <class T>
	inline typename Vector<T>::reference Vector<T>::back()
	{
		return *(mEnd-1);
	}

	template <class T>
	inline typename Vector<T>::const_reference Vector<T>::back() const
	{
		return *(mEnd-1);
	}
	
	template <class T>
	inline typename Vector<T>::reference Vector<T>::at(size_t n)
	{
		return *(mBegin+n);
	}

	template <class T>
	inline typename Vector<T>::const_reference Vector<T>::at(size_t n) const
	{
		return *(mBegin+n);
	}

	template <class T>
	inline typename Vector<T>::reference Vector<T>::operator[](size_t n)
	{
		return *(mBegin+n);
	}

	template <class T>
	inline typename Vector<T>::const_reference Vector<T>::operator[](size_t n) const
	{
		return *(mBegin+n);
	}

	template <class T>
	inline size_t Vector<T>::capacity() const
	{
		return (size_t)(mCapacity - mBegin);
	}

	template <class T>
	inline size_t Vector<T>::size() const
	{
		return (size_t)(mEnd - mBegin);
	}

	template <class T>
	inline bool Vector<T>::empty() const
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

			mAllocator->Deallocate(mBegin);

			mCapacity = mEnd = mBegin = NULL;
		}
	}

	template <class T>
	inline void Vector<T>::assign(iterator first, iterator last)
	{
		clear();

		insert(mBegin,first,last);
	}
	
	template <class T>
	inline void Vector<T>::assign(size_t size, const T& value)
	{
		clear();

		insert(mBegin,size,value);
	}

	template <class T>
	inline typename Vector<T>::iterator 
	Vector<T>::insert(iterator position, const T& value)
	{
		const size_t pos = (position - mBegin);

		if (mEnd != mCapacity)
		{
			memcpy(mBegin+pos+1, mBegin+pos, sizeof(T) * (mEnd-position));
			memcpy(mBegin+pos, &value, sizeof(T));

			mEnd++;

			return mBegin+pos;
		}
		else
		{
			const size_t prevSize = mEnd - mBegin;
			const size_t newSize = GetNewCapacity(prevSize);
			T* newBegin = (T*) mAllocator->Allocate(newSize * sizeof(T));

			memcpy(newBegin, mBegin, sizeof(T)*pos);
			memcpy(newBegin+pos, &value, sizeof(T));
			memcpy(newBegin+pos+1, mBegin+pos, (mEnd - position) * sizeof(T));
			mAllocator->Deallocate(mBegin);

			mBegin = newBegin;
			mEnd = mBegin + prevSize + 1;
			mCapacity = mBegin + newSize;

			return mBegin+pos;
		}
	}
	
	template <class T>
	inline void Vector<T>::insert(iterator position, size_t n, const T& value)
	{
  		const size_t pos = (position - mBegin);

		if (n>0)
		{
			if (n <= uint32_t(mCapacity - mEnd))
			{
				memcpy(mBegin+pos+n, mBegin+pos, sizeof(T) * (mEnd - position));

				for (uint32_t i = 0; i<n; i++)
					memcpy(mBegin+pos+i, &value, sizeof(T));

				mEnd += n;
			}
			else
			{
				const size_t prevSize = mEnd - mBegin;
				const size_t newSize = GetNewCapacity(prevSize, n);
				T* newBegin = (T*) mAllocator->Allocate(newSize * sizeof(T));

				memcpy(newBegin, mBegin, sizeof(T) * pos);
				for (uint32_t i = 0; i<n; i++)
					memcpy(newBegin+pos+i, &value, sizeof(T));
				memcpy(newBegin+pos+n,mBegin+pos, (mEnd - position) * sizeof(T));

				if (mBegin)
					mAllocator->Deallocate(mBegin);

				mBegin = newBegin;
				mEnd = mBegin + prevSize + n;
				mCapacity = mBegin + newSize;
			}
		}
	}
	
	template <class T>
	inline void Vector<T>::insert(iterator position,iterator first, iterator last)
	{
		const size_t insertPos = (position - mBegin);
		const size_t insertSize = (last - first);

		if (insertSize <= size_t(mCapacity - mEnd))
		{
			memcpy(mBegin+insertPos + insertSize, mBegin + insertPos, (mEnd - position) * sizeof(T));
			
			for (iterator i = first; i < last && i != mEnd; i++)
			{
				memcpy(position, i, sizeof(T));
				position++;
			}
			mEnd += insertSize;
		}
		else
		{
			const size_t prevSize = mEnd - mBegin;
			const size_t pos = (position - mBegin);
			const size_t newSize = GetNewCapacity(prevSize, insertSize);
			T* newBegin = (T*) mAllocator->Allocate(newSize * sizeof(T));

			memcpy(newBegin,mBegin, pos * sizeof(T));
			memcpy(newBegin+pos, first, insertSize * sizeof(T));
			memcpy(newBegin+pos+insertSize, mBegin+pos, (mEnd - position) * sizeof(T));

			if (mBegin)
				mAllocator->Deallocate(mBegin);

			mBegin = newBegin;
			mEnd = mBegin + prevSize + insertSize;
			mCapacity = mBegin + newSize;
		}
	}

	template <class T>
	inline typename Vector<T>::iterator 
	Vector<T>::erase(iterator position)
	{
		if (position != mEnd)
		{
			const size_t locPos = (position - mBegin);

			position->~T();
			memcpy(mBegin + locPos, mBegin + locPos + 1, sizeof(T) * (mEnd - position));

			mEnd--;
	
			return mBegin+locPos;
		}
		else
			return mEnd;
	}

	template <class T>
	inline typename Vector<T>::iterator 
	Vector<T>::erase(iterator first, iterator end)
	{
		const size_t locBegin = (first - mBegin);
		const size_t locEnd = (end - mBegin);

		for (iterator i = first; i != end; i++)
			i->~T();

		memcpy(mBegin+locBegin, mBegin + locEnd + 1, sizeof(T) * (mEnd - end));

		mEnd -= (end-first);

		return mBegin + locBegin;
	}

	template <class T>
	inline void Vector<T>::resize(size_t numElements,T obj)
	{
		const size_t prevSize = mEnd - mBegin;
		
		if (prevSize < numElements)
			insert(mEnd, numElements - prevSize, obj);
		else
			erase(mBegin+numElements, mEnd);
	}



	template <class T>
	inline void Vector<T>::push_back(const T& item)
	{
		if (mEnd != mCapacity)
		{
			memcpy(mEnd, &item, sizeof(T));
			
			mEnd++;
		}
		else
		{
			const size_t prevSize = mEnd - mBegin;
			const size_t newSize = GetNewCapacity(prevSize);

			T* newBegin = (T*) mAllocator->Allocate(newSize * sizeof(T));
			memcpy(newBegin, mBegin, prevSize * sizeof(T));
			mAllocator->Deallocate(mBegin);

			mBegin = newBegin;
			mEnd = mBegin + prevSize;
			mCapacity = mBegin + newSize;

			memcpy(mEnd, &item, sizeof(T));
			
			mEnd++;
		}
	}

	template <class T>
	inline void Vector<T>::pop_back()
	{
		(mEnd - 1)->~T();
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
		return (currentCapacity >= 0 && currentCapacity*DefaultCapacityIncrease <= minimum+currentCapacity) ? (minimum+currentCapacity) : currentCapacity*DefaultCapacityIncrease;
	}

}

#endif