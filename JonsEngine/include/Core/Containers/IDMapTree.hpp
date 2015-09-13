#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>

namespace JonsEngine
{
    //
    // TODO: extend with all/immediate child iterators when needed
    //

    template <typename T>
    class IDMapTree// : private IDMap<T>
    {
    public:
        typedef uint32_t ItemID;
        const static ItemID INVALID_ITEM_ID = 0;

    private:
        struct Item {
            ItemID mNext;
            T mItem;

            template <typename... Arguments>
            Item(const ItemID next, Arguments&&... args);
            Item(Item& other);
        };

    public:
        template <typename... Arguments>
        IDMapTree(Arguments&&... args);
        ~IDMapTree();

        template <typename... Arguments>
        ItemID AddNode(const ItemID parent, Arguments&&... args);
        void FreeNode(ItemID& nodeID);
        
        ItemID GetRootNodeID() const;
        T& GetNode(const ItemID nodeID);
        const T& GetNode(const ItemID nodeID) const;

        size_t Size() const;
        size_t Capacity() const;


    private:
        void Grow();
        Item& GetItem(const ItemID nodeID);


        IMemoryAllocator& mAllocator;
        ItemID mIDCounter;
        ItemID mRootNodeID;

        Item* mBegin;
        Item* mEnd;
        Item* mCapacity;
        std::vector<uint32_t> mIndirectionLayer;
    };


    //
    // IDMapTree::Item
    //

    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::Item::Item(const ItemID next, Arguments&&... args) : mNext(next), mItem(std::forward<Arguments>(args)...)
    {
    }

    template <typename T>
    IDMapTree<T>::Item::Item(Item& other) : mNext(other.mNext), mItem(other.mItem)
    {
    }


    //
    // IDMapTree
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::IDMapTree(Arguments&&... args) :
        mAllocator(HeapAllocator::GetDefaultHeapAllocator()),
        mIDCounter(1),
        mRootNodeID(mIDCounter++),
        mBegin(nullptr),
        mEnd(nullptr),
        mCapacity(nullptr)
    {
        // Construct the root node
        Grow();
        new (mEnd++) Item(INVALID_ITEM_ID, std::forward<Arguments>(args)...);
    }

    template <typename T>
    IDMapTree<T>::~IDMapTree()
    {
        if (mBegin)
            mAllocator.Deallocate(mBegin);
    }


    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(const ItemID parentID, Arguments&&... args)
    {
        // rare case; could reorder statement for performance?
        // but insertion is expensive and preferably rare anyway so dont bother
        if (mEnd >= mCapacity)
            Grow();
        
        Item& parent = GetItem(parentID);
        const ItemID parentNextID = parent.mNext;
        Item* parentNextNode = parentNextID != INVALID_ITEM_ID ? &GetItem(parentNextID) : mEnd;
        Item* newChildNode = &parent + 1;

        // find parents next
        while (newChildNode != parentNextNode)
            newChildNode++;

        // move all elements one index to make room for new child
        std::move_backward(newChildNode, mEnd, mEnd);

        //             [1]
        //      [2]             
        // [3]       [4]

        //const Item* newChild = mEnd;
        const ItemID newChildID = mIDCounter++;
        new (newChildNode)Item(parentNextID, std::forward<Arguments>(args)...);
        mEnd++;
        
        Item* currChild = &parent + 1;
        if (currChild == newChildNode)
            return newChildID;

        // if parent has other children, update the last one to point to this one
        while (currChild->mNext != parentNextID)
            currChild = &GetItem(currChild->mNext);
        currChild->mNext = newChildID;

        return newChildID;
    }

    template <typename T>
    void IDMapTree<T>::FreeNode(ItemID& nodeID)
    {
        Item* i = mBegin;
        Item* i2 = mBegin + 1;
        Item* i3 = mBegin + 2;
        Item* i4 = mBegin + 3;
    }


    template <typename T>
    typename IDMapTree<T>::ItemID IDMapTree<T>::GetRootNodeID() const
    {
        return mRootNodeID;
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
        Item* newBuffer = static_cast<Item*>(mAllocator.Allocate(newCapacity * sizeof(Item)));

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

        const uint32_t index = nodeID - 1;

        return *(mBegin + index);
    }
}