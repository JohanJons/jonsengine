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
            T mItem;
            Item* mNext;

            template <typename... Arguments>
            Item(Arguments&&... args);
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
        
        T& GetNode(const ItemID nodeID);
        const T& GetNode(const ItemID nodeID) const;

        size_t Size() const;
        size_t Capacity() const;


    private:
        void Grow();
        Item& GetItem(const ItemID nodeID);


        IMemoryAllocator& mAllocator;
        ItemID mIDCounter;

        Item* mBegin;
        Item* mEnd;
        Item* mCapacity;
        std::vector<uint32_t> mIndirectionLayer;
    };



    //
    // IDMapTree
    //

    template <typename T>
    IDMapTree<T>::IDMapTree() :
        mAllocator(HeapAllocator::GetDefaultHeapAllocator()),
        mIDCounter(1),
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
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(const ItemID parentID, Arguments&&... args)
    {
        // rare case; could reorder statement for performance?
        if (mEnd >= mCapacity)
            Grow();
        
        const Item& parent = GetItem(parentID);

        //const uint16_t initialVersion = 1;
        new (mEnd++) Item(std::forward<Arguments>(args)...);
        
        return mIDCounter++;// | (static_cast<uint32_t>(initialVersion) << 16));

    }

    template <typename T>
    void IDMapTree<T>::FreeNode(const ItemID nodeID)
    {
        Item* i = mBegin;
        Item* i2 = mBegin + 1;
        Item* i3 = mBegin + 2;
    }


    template <typename T>
    T& IDMapTree<T>::GetNode(const ItemID nodeID)
    {
        return GetItem(nodeID)->mItem;
    }

    template <typename T>
    const T& IDMapTree<T>::GetNode(const ItemID nodeID) const
    {
        return GetItem(nodeID)->mItem;
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
        Item* newBuffer = static_cast<Item*>(mAllocator.Allocate(newCapacity));

        // copy prev elements
        std::uninitialized_copy(mBegin, mEnd, newBuffer);
        
        // destroy old elements
        std::for_each(mBegin, mEnd, [](Item& item) { item.~Item(); });
        mAllocator.Deallocate(mBegin);

        mBegin = newBuffer;
        mEnd = mBegin + prevSize;
        mCapacity = mBegin + newCapacity;
    }

    template <typename T>
    typename IDMapTree<T>::Item& IDMapTree<T>::GetItem(const ItemID nodeID)
    {
        assert(nodeID != INVALID_ITEM_ID);

        const uint32_t index = nodeID;

        return *(mBegin + index);
    }
}