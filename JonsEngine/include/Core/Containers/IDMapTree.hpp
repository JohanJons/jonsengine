#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>
#include <limits>
#include <functional>

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
            ItemID mID;
            ItemID mNext;
            T mItem;

            template <typename... Arguments>
            Item(const ItemID id, const ItemID next, Arguments&&... args);
        };

        typedef typename std::vector<Item> ItemContainer;
        typedef typename ItemContainer::iterator ItemIterator;

    public:
        class iterator
        {
        public:
            iterator(typename const ItemIterator& iter);

            iterator& operator++();
            iterator operator++(int);

            void operator+=(const size_t offset);
            iterator operator+ (const size_t offset);

            T& operator*();
            const T& operator*() const;

            bool operator!=(const iterator& iter) const;


        private:
            typename ItemIterator mIterator;
        };

        class ImmediateChildrenIterator : public iterator
        {
        public:
            typedef std::function<ItemIterator(const ItemID itemID)> GetNodeFunc;

            ImmediateChildrenIterator(const iterator& iter, const GetNodeFunc& getNodeFunc);

            ImmediateChildrenIterator& operator++();
            
            ImmediateChildrenIterator& operator--() = delete;
            ImmediateChildrenIterator operator--(int) = delete;
            ImmediateChildrenIterator& operator+=(typename ItemIterator::difference_type) = delete;
            ImmediateChildrenIterator operator+(typename ItemIterator::difference_type) const = delete;
            ImmediateChildrenIterator& operator-=(typename ItemIterator::difference_type) = delete;


        private:
            GetNodeFunc mGetNodeFunc;
        };

    public:
        typename typedef RangedIterator<std::vector<T>, ImmediateChildrenIterator> ImmChildIterator;

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

        iterator GetAllChildrenIterator(const ItemID nodeID);
        ImmChildIterator GetImmediateChildrenIterator(const ItemID nodeID);


    private:
        ItemIterator GetItem(const ItemID nodeID);
        ItemID& GetIndirectionID(const ItemID publicID);


        ItemID mRootNodeID;

        ItemContainer mItems;
        std::vector<ItemID> mIndirectionLayer;
        std::vector<uint16_t> mFreeIndirectionIndices;
    };



    //
    // IDMapTree::Item
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::Item::Item(const ItemID id, const ItemID next, Arguments&&... args) : mID(id), mNext(next), mItem(std::forward<Arguments>(args)...)
    {
    }


    //
    // IDMapTree::Iterator
    //
    template <typename T>
    IDMapTree<T>::iterator::iterator(typename const ItemIterator& iter) : mIterator(iter)
    {
    }

    template <typename T>
    bool IDMapTree<T>::iterator::operator!=(const iterator& iter) const
    {
        return mIterator != iter.mIterator;
    }

    template <typename T>
    typename IDMapTree<T>::iterator& IDMapTree<T>::iterator::operator++()
    {
        ++mIterator;

        return *this;
    }

    template <typename T>
    typename IDMapTree<T>::iterator IDMapTree<T>::iterator::operator++(int)
    {
        iterator old(++(*this));

        return old;
    }

    template <typename T>
    void IDMapTree<T>::operator+=(const size_t offset)
    {

    }

    template <typename T>
    typename IDMapTree<T>::iterator IDMapTree<T>::operator+(const size_t offset)
    {

    }


    template <typename T>
    T& IDMapTree<T>::iterator::operator*()
    {
        return mIterator->mItem;
    }

    template <typename T>
    const T& IDMapTree<T>::iterator::operator*() const
    {
        return mIterator->mItem;
    }


    //
    // IDMapTree::ImmediateChildrenIterator
    //
    template <typename T>
    IDMapTree<T>::ImmediateChildrenIterator::ImmediateChildrenIterator(const iterator& iter, const GetNodeFunc& getNodeFunc) : iterator(iter), mGetNodeFunc(getNodeFunc)
    {
    }

    template <typename T>
    typename IDMapTree<T>::ImmediateChildrenIterator& IDMapTree<T>::ImmediateChildrenIterator::operator++()
    {
        this->_Ptr = this->_Ptr->mNext._Ptr;

        return *this;
    }


    //
    // IDMapTree
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTree<T>::IDMapTree(Arguments&&... args) : mRootNodeID(INVALID_ITEM_ID)
    {
        // Construct the root node
        const uint16_t rootNodeIndex = 0;
        const uint16_t version = 1;

        mRootNodeID = rootNodeIndex | (static_cast<uint32_t>(version) << 16);
        mIndirectionLayer.emplace_back(mRootNodeID);
        mItems.emplace_back(mRootNodeID, INVALID_ITEM_ID, std::forward<Arguments>(args)...);
    }

    template <typename T>
    IDMapTree<T>::~IDMapTree()
    {
    }


    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(const ItemID parentID, Arguments&&... args)
    {
        assert(mItems.size() < std::numeric_limits<uint16_t>().max());

        ItemIterator parentIter = GetItem(parentID);
        const ItemID parentNextID = parentIter->mNext;
        ItemIterator endIter = mItems.end();
        ItemIterator parentNextIter = parentNextID != INVALID_ITEM_ID ? GetItem(parentNextID) : endIter;
        ItemIterator newChildIter = parentIter + 1;

        while (newChildIter != parentNextIter)
            ++newChildIter;

        const uint16_t newChildIndex = newChildIter - mItems.cbegin();
        ItemID publicChildID = INVALID_ITEM_ID;
        if (!mFreeIndirectionIndices.empty())
        {
            const uint16_t freeIndirectionIndex = mFreeIndirectionIndices.back();
            mFreeIndirectionIndices.pop_back();

            ItemID& indirectionID = mIndirectionLayer.at(freeIndirectionIndex);
            const uint16_t version = IDMAP_VERSION_MASK(indirectionID) + 1;
            indirectionID = newChildIndex | (static_cast<uint32_t>(version) << 16);

            publicChildID = freeIndirectionIndex | (static_cast<uint32_t>(version) << 16);
        }
        else
        {
            const uint16_t version = 1;
            const ItemID indirectionID = newChildIndex | (static_cast<uint32_t>(version) << 16);
            mIndirectionLayer.emplace_back(indirectionID);

            publicChildID = (mIndirectionLayer.size() - 1) | (static_cast<uint32_t>(version) << 16);
        }

        ItemIterator firstChildIter = parentIter + 1;
        // if parent already has a child
        if (firstChildIter->mID != parentNextID)
        {
            // if parent has more than one children
            if (firstChildIter->mNext != parentNextID)
            {
                do
                    ++firstChildIter;
                while (firstChildIter->mNext != parentNextID);
            }

            firstChildIter->mNext = publicChildID;
        }

        // increment indirection layer indexes
        std::for_each(newChildIter, mItems.end(), [this](Item& item) { ItemID& indirID = GetIndirectionID(item.mID); ++indirID; });
            
        mItems.emplace(newChildIter, publicChildID, parentNextID, std::forward<Arguments>(args)...);

        return publicChildID;
    }

    template <typename T>
    void IDMapTree<T>::FreeNode(ItemID& nodeID)
    {
        ItemIterator node = GetItem(nodeID);
        ItemIterator endNode = node->mNext != INVALID_ITEM_ID ? GetItem(node->mNext) : mItems.end();

        // add node and all children to free index list
        std::for_each(node, endNode, [this](Item& item) { const uint16_t index = IDMAP_INDEX_MASK(item.mID); mFreeIndirectionIndices.push_back(index); });
        std::for_each(mItems.begin(), node, [this, node](Item& item) { if (item.mNext == node->mID) item.mNext = node->mNext; });

        mItems.erase(node, endNode);

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
    typename IDMapTree<T>::iterator IDMapTree<T>::begin()
    {
        return iterator(mItems.begin());
    }

    template <typename T>
    typename IDMapTree<T>::iterator IDMapTree<T>::end()
    {
        return iterator(mItems.end());
    }


    template <typename T>
    typename IDMapTree<T>::ItemIterator IDMapTree<T>::GetItem(const ItemID nodeID)
    {
        const ItemID& itemID = GetIndirectionID(nodeID);
        const uint16_t itemIndex = IDMAP_INDEX_MASK(itemID);

        assert(itemIndex <= mItems.size());

        return mItems.begin() + itemIndex;
    }

    template <typename T>
    typename IDMapTree<T>::ItemID& IDMapTree<T>::GetIndirectionID(const ItemID publicID)
    {
        assert(publicID != INVALID_ITEM_ID);

        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(publicID);
        const uint16_t version = IDMAP_VERSION_MASK(publicID);

        ItemID& indirID = mIndirectionLayer.at(indirectionIndex);

        assert(version == IDMAP_VERSION_MASK(indirID));

        return indirID;
    }


    template <typename T>
    typename IDMapTree<T>::iterator IDMapTree<T>::GetAllChildrenIterator(const ItemID nodeID)
    {

    }

    template <typename T>
    typename IDMapTree<T>::ImmChildIterator IDMapTree<T>::GetImmediateChildrenIterator(const ItemID nodeID)
    {
//        ItemIterator node = GetNode(nodeID);

 //       return ImmChildIterator(node);
    }
}