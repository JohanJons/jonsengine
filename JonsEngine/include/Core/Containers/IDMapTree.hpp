#pragma once

#include "include/Core/Containers/IDMapBase.hpp"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Memory/HeapAllocator.h"

#include <algorithm>
#include <limits>
#include <functional>

namespace JonsEngine
{
    // Internal storage item
    template <typename T>
    struct IDMapTreeItem
    {
        ItemID mID;
        ItemID mNext;
        T mItem;

        template <typename... Arguments>
        IDMapTreeItem(const ItemID id, const ItemID next, Arguments&&... args);
    };

    // Contigous-memory ID-based tree container
    // Elements must be MoveAssignable
    // TODO: Extend with all/immediate child iterators
    template <typename T>
    class IDMapTree : public IDMapBase<T, IDMapTreeItem<T>>
    {
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
        ItemID mRootNodeID;
    };



    //
    // IDMapTreeItem
    //
    template <typename T>
    template <typename... Arguments>
    IDMapTreeItem<T>::IDMapTreeItem(const ItemID id, const ItemID next, Arguments&&... args) : mID(id), mNext(next), mItem{ std::forward<Arguments>(args)... }
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

        ItemIterator parentIter = GetItemIter(parentID);
        const ItemID parentNextID = parentIter->mNext;
        ItemIterator endIter = mItems.end();
        ItemIterator parentNextIter = parentNextID != INVALID_ITEM_ID ? GetItemIter(parentNextID) : endIter;
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
        ItemIterator node = GetItemIter(nodeID);
        ItemIterator endNode = node->mNext != INVALID_ITEM_ID ? GetItemIter(node->mNext) : mItems.end();

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
        return GetItemIter(nodeID)->mItem;
    }

    template <typename T>
    const T& IDMapTree<T>::GetNode(const ItemID nodeID) const
    {
        return GetItemIter(nodeID)->mItem;
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
}