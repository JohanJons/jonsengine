#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>

namespace JonsEngine
{
    template <typename T>
    class IDMapTree// : private IDMap<T>
    {
    private:
        struct Item {
            uint16_t mVersion;
            T mItem;

            template <typename... Arguments>
            Item(uint16_t version, Arguments&&... args);
        };

    public:
        typedef uint32_t ItemID;
        const static ItemID INVALID_ITEM_ID = 0;

        IDMapTree();
        ~IDMapTree();

        template <typename... Arguments>
        ItemID AddNode(Arguments&&... args);
        template <typename... Arguments>
        ItemID AddNode(const ItemID parent, Arguments&&... args);
        void FreeNode(const ItemID nodeID);

        size_t Size() const;
        size_t Capacity() const;


    private:
        void Grow();


        IMemoryAllocator& mAllocator;

        T* mBegin;
        T* mEnd;
        T* mCapacity;
        std::vector<uint32_t> mIndirectionLayer;
    };



    //
    // IDMapTree
    //

    template <typename T>
    IDMapTree<T>::IDMapTree() :
        mAllocator(HeapAllocator::GetDefaultHeapAllocator()),
        mBegin(nullptr),
        mEnd(nullptr),
        mCapacity(nullptr)
    {
    }

    template <typename T>
    IDMapTree<T>::~IDMapTree()
    {
        if (mBegin)
            mAllocator.Deallocate(mBegin);
    }


    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(Arguments&&... args)
    {
        return AddNode(INVALID_ITEM_ID, std::forward<Arguments>(args)...);
    }

    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(const ItemID parent, Arguments&&... args)
    {
        // rare case; could reorder statement for performance?
        if (mEnd >= mCapacity)
            Grow();
        
        const uint16_t initialVersion = 1;
        new (mEnd++) T( std::forward<Arguments>(args)...);
        
        return ((Size() - 1) | (static_cast<uint32_t>(initialVersion) << 16));

    }

    template <typename T>
    void IDMapTree<T>::FreeNode(const ItemID nodeID)
    {
        T* i = mBegin;
        T* i2 = mBegin + 1;
        T* i3 = mBegin + 2;
    }


    template <typename T>
    size_t IDMapTree<T>::Size() const
    {
        return static_cast<size_t>(mEnd - mBegin);
    }

    template <typename T>
    size_t IDMapTree<T>::Capacity() const
    {
        return static_cast<size_t>(mCapacity - mBegin);
    }


    template <typename T>
    void IDMapTree<T>::Grow()
    {
        const size_t prevCapacity = Capacity();
        const size_t prevSize = Size();
        const size_t newCapacity = prevCapacity != 0 ? static_cast<size_t>(1.5f * prevCapacity) : 2;
        T* newBuffer = static_cast<T*>(mAllocator.Allocate(newCapacity));

        // copy prev elements
        std::uninitialized_copy(mBegin, mEnd, newBuffer);
        
        // destroy old elements
        std::for_each(mBegin, mEnd, [](T& item) { item.~T(); });
        mAllocator.Deallocate(mBegin);

        mBegin = newBuffer;
        mEnd = mBegin + prevSize;
        mCapacity = mBegin + newCapacity;
    }
}