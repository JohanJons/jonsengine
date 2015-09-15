#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>
#include <limits>

namespace JonsEngine
{
    //
    // TODO: extend with all/immediate child iterators when needed
    //

    // Contigous-memory ID-based tree container
    // Elements must be MoveAssignable
    template <typename T>
    class IDMapTree// : private IDMap<T>
    {
    public:
        typedef uint32_t ItemID;
        const static ItemID INVALID_ITEM_ID = 0;

    private:
        struct Item
        {
            ItemID mNext;
            T mItem;

            template <typename... Arguments>
            Item(const ItemID next, Arguments&&... args);
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

        void Clear();

        size_t Size() const;
        size_t Capacity() const;


    private:
        typedef typename std::vector<Item> ItemContainer;
        typedef typename ItemContainer::iterator ItemIterator;

        ItemIterator GetItem(const ItemID nodeID);


        ItemID mRootNodeID;

        ItemContainer mItems;
        std::vector<ItemID> mIndirectionLayer;
    };



    //
    // IDMapTree::Item
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::Item::Item(const ItemID next, Arguments&&... args) : mNext(next), mItem(std::forward<Arguments>(args)...)
    {
    }


    //
    // IDMapTree
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::IDMapTree(Arguments&&... args) : mRootNodeID(INVALID_ITEM_ID)
    {
        // Construct the root node
        mItems.emplace_back(INVALID_ITEM_ID, std::forward<Arguments>(args)...);

        mRootNodeID = (mItems.size() - 1) | (static_cast<uint32_t>(1) << 16);
        mIndirectionLayer.emplace_back(mRootNodeID);
    }

    template <typename T>
    IDMapTree<T>::~IDMapTree()
    {
    }


    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(const ItemID parentID, Arguments&&... args)
    {
        /*Item& parent = GetItem(parentID);
        const ItemID parentNextID = parent.mNext;
        Item* end = &mItems.back();
        Item* parentNextNode = parentNextID != INVALID_ITEM_ID ? &GetItem(parentNextID) : end;
        Item* newChildNode = &parent + 1;

        // find parents next
        while (newChildNode != parentNextNode)
            newChildNode++;*/

        // move all elements one index to make room for new child
        //std::move_backward(newChildNode, end, end);

        //             [1]
        //      [2]             
        // [3]       [4]

        assert(mItems.size() < std::numeric_limits<uint16_t>().max());

        ItemIterator parentIter = GetItem(parentID);
        const ItemID parentNextID = parentIter->mNext;
        ItemIterator endIter = mItems.end();
        ItemIterator parentNextIter = parentNextID != INVALID_ITEM_ID ? GetItem(parentNextID) : endIter;
        ItemIterator newChildIter = parentIter + 1;

        while (newChildIter != parentNextIter)
            ++newChildIter;

        mItems.emplace(newChildIter, parentNextID, std::forward<Arguments>(args)...);

        const uint16_t newChildIndex = newChildIter - mItems.cbegin();
        //uint16_t newChildVersion = 1;
        //ItemID newChildID = INVALID_ITEM_ID;
        //if (mIndirectionLayer.size() < mItems.size())
        //{

        //    mIndirectionLayer.emplace_back();
        //}
        //else
        //{

        //}
        

        const uint16_t newChildIndex = newChildIter - mItems.cbegin();
        const ItemID newChildID = newChildIndex;
        //new (newChildNode)Item(parentNextID, std::forward<Arguments>(args)...);
        //mEnd++;

        ItemIterator currChildIter = parentIter + 1;
        if (currChildIter == newChildIter)
            return newChildID;

        // if parent has other children, update the last one to point to this one
        while (currChildIter->mNext != parentNextID)
            ++currChildIter;
        currChildIter->mNext = newChildID;

        return newChildID;
    }

    template <typename T>
    void IDMapTree<T>::FreeNode(ItemID& nodeID)
    {
        //Item* i = mBegin;
        //Item* i2 = mBegin + 1;
        //Item* i3 = mBegin + 2;
        //Item* i4 = mBegin + 3;

        ItemIterator beginNode = GetItem(nodeID);
        ItemIterator endNode = GetItem(beginNode->mNext);
        mItems.erase(beginNode, endNode);

        nodeID = INVALID_ITEM_ID;
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
    void IDMapTree<T>::Clear()
    {
        mItems.clear();
    }


    template <typename T>
    size_t IDMapTree<T>::Size() const
    {
        return mItems.size();
    }

    template <typename T>
    size_t IDMapTree<T>::Capacity() const
    {
        return mItems.capacity();
    }


    template <typename T>
    typename IDMapTree<T>::ItemIterator IDMapTree<T>::GetItem(const ItemID nodeID)
    {
        assert(nodeID != INVALID_ITEM_ID);

        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(nodeID);
        const uint16_t version = IDMAP_VERSION_MASK(nodeID);

        const ItemID itemID = mIndirectionLayer.at(indirectionIndex);
        const uint16_t itemIndex = IDMAP_INDEX_MASK(itemID);
        const uint16_t itemVersion = IDMAP_VERSION_MASK(itemID);

        assert(version == itemVersion);
        assert(itemIndex <= mItems.size());

        return mItems.begin() + itemIndex;
    }


    /*template <typename T>
    class IDMapTree// : private IDMap<T>
    {
    public:
        typedef uint32_t ItemID;
        const static ItemID INVALID_ITEM_ID = 0;

    private:
        struct Item
        {
            ItemID mNext;
            T mItem;

            template <typename... Arguments>
            Item(const ItemID next, Arguments&&... args);
        };

    public:
        class iterator
        {
        public:
            iterator(typename Item* pointer);

            bool operator!=(const iterator& iter) const;
            iterator& operator++();
            iterator operator++(int);
            T& operator*();
            const T& operator*() const;


        private:
            typename Item* mPointer;
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

        void Clear();

        size_t Size() const;
        size_t Capacity() const;

        iterator begin();
        iterator end();


    private:
        void Grow();
        Item& GetItem(const ItemID nodeID);


        IMemoryAllocator& mAllocator;
        ItemID mIDCounter;
        ItemID mRootNodeID;

        Item* mBegin;
        Item* mEnd;
        Item* mCapacity;
        std::vector<ItemID> mIndirectionLayer;
    };


    //
    // IDMapTree::Item
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::Item::Item(const ItemID next, Arguments&&... args) : mNext(next), mItem(std::forward<Arguments>(args)...)
    {
    }


    //
    // IDMapTree::Iterator
    //
    template <typename T>
    IDMapTree<T>::iterator::iterator(typename Item* pointer) : mPointer(pointer)
    {
    }

    template <typename T>
    bool IDMapTree<T>::iterator::operator!=(const iterator& iter) const
    {
        return mPointer != iter.mPointer;
    }

    template <typename T>
    typename IDMapTree<T>::iterator& IDMapTree<T>::iterator::operator++()
    {
        ++mPointer;

        return *this;
    }

    template <typename T>
    typename IDMapTree<T>::iterator IDMapTree<T>::iterator::operator++(int)
    {
        iterator old(++(*this));

        return old;
    }

    template <typename T>
    T& IDMapTree<T>::iterator::operator*()
    {
        return mIterator->mPointer;
    }

    template <typename T>
    const T& IDMapTree<T>::iterator::operator*() const
    {
        return mIterator->mPointer;
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

        const ItemID newChildID = mIDCounter++;
        new (newChildNode) Item(parentNextID, std::forward<Arguments>(args)...);
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

        Item* freedNode = &GetItem(nodeID);
        Item* endNode = &GetItem(freedNode->mNext);
        
        // destruct node and its children and defragment container
        std::for_each(freedNode, endNode, [](Item& node) { node.~Item(); });
        std::move(endNode, mEnd, freedNode);

        const uint32_t numItemsRemoved = endNode - freedNode;
        mEnd -= numItemsRemoved;

        nodeID = INVALID_ITEM_ID;
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
    void IDMapTree<T>::Clear()
    {
        std::for_each(mBegin, mEnd, [](Item& node){ node.~Item(); });
        mEnd = mBegin;
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

        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(nodeID);
        const uint16_t version = IDMAP_VERSION_MASK(nodeID);

        const ItemID itemID = mIndirectionLayer.at(indirectionIndex);
        const uint16_t itemIndex = IDMAP_INDEX_MASK(itemID);
        const uint16_t itemVersion = IDMAP_VERSION_MASK(itemID);

        assert(version == itemVersion);

        return *(mBegin + itemIndex);
    }*/
}