#pragma once

#include "include/Core/Types.h"

#include <vector>
#include <assert.h>

namespace JonsEngine
{
    typedef uint32_t ItemID;

    // base class for ID-based map containers
    // TODO: specify StorageType must have mItem...
    template<typename T, typename StorageType>
    class IDMapBase
    {
    public:
        typedef typename StorageType Item;
        typedef typename std::vector<Item>::iterator ItemIterator;

        // this is admittedly rather unfortunate
        typedef ItemID ItemID;
        const static ItemID INVALID_ITEM_ID = 0;

        // TODO: maybe extend to adhere to RandomAccessIterator
        class iterator
        {
        public:
            iterator(typename const ItemIterator& iter);

            bool operator!=(const iterator& iter) const;

            iterator& operator++();
            iterator operator++(int);

            void operator+=(const size_t offset);
            iterator operator+ (const size_t offset);

            T& operator*();
            const T& operator*() const;


        private:
            ItemIterator mIterator;
        };


    protected:
        ItemIterator GetItemIter(const ItemID nodeID);
        ItemID& GetIndirectionID(const ItemID publicID);


        std::vector<Item> mItems;
        std::vector<ItemID> mIndirectionLayer;
        std::vector<uint16_t> mFreeIndirectionIndices;
    };
    
    #define IDMAP_INDEX_MASK(id) (id & 0xFFFFFFFF)
    #define IDMAP_VERSION_MASK(id) ((id >> 16) & 0xFFFFFFFF)


    //
    // IDMapBase::iterator
    //

    template <typename T, typename StorageType>
    IDMapBase<T, StorageType>::iterator::iterator(typename const ItemIterator& iter) : mIterator(iter)
    {
    }

    template <typename T, typename StorageType>
    bool IDMapBase<T, StorageType>::iterator::operator!=(const iterator& iter) const
    {
        return mIterator != iter.mIterator;
    }

    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::iterator& IDMapBase<T, StorageType>::iterator::operator++()
    {
        ++mIterator;

        return *this;
    }

    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::iterator IDMapBase<T, StorageType>::iterator::operator++(int)
    {
        iterator old(++(*this));

        return old;
    }

    template <typename T, typename StorageType>
    T& IDMapBase<T, StorageType>::iterator::operator*()
    {
        return mIterator->mItem;
    }

    template <typename T, typename StorageType>
    const T& IDMapBase<T, StorageType>::iterator::operator*() const
    {
        return mIterator->mItem;
    }


    //
    // IDMapBase
    //
    
    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::ItemIterator IDMapBase<T, StorageType>::GetItemIter(const ItemID nodeID)
    {
        const ItemID& itemID = GetIndirectionID(nodeID);
        const uint16_t itemIndex = IDMAP_INDEX_MASK(itemID);

        assert(itemIndex <= mItems.size());

        return mItems.begin() + itemIndex;
    }
    
    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::ItemID& IDMapBase<T, StorageType>::GetIndirectionID(const ItemID publicID)
    {
        assert(publicID != INVALID_ITEM_ID);

        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(publicID);
        const uint16_t version = IDMAP_VERSION_MASK(publicID);

        ItemID& indirID = mIndirectionLayer.at(indirectionIndex);

        assert(version == IDMAP_VERSION_MASK(indirID));

        return indirID;
    }
    

    /*template <typename T, typename StorageType>
    void IDMapBase<T, StorageType>::FreeItem(ItemID& nodeID)
    {
        //ItemID& indirID = GetIndirectionID(nodeID);
        
        const uint16_t indirIndex = IDMAP_INDEX_MASK(nodeID);
        mFreeIndirectionIndices.push_back(indirIndex);
        
        ItemIterator item = GetItem(nodeID);
        mItems.erase(item);
        
        //ItemIterator endNode = node->mNext != INVALID_ITEM_ID ? GetItem(node->mNext) : mItems.end();

        // add node and all children to free index list
        //std::for_each(node, endNode, [this](Item& item) { const uint16_t index = IDMAP_INDEX_MASK(item.mID); mFreeIndirectionIndices.push_back(index); });
        //std::for_each(mItems.begin(), node, [this, node](Item& item) { if (item.mNext == node->mID) item.mNext = node->mNext; });

        //mItems.erase(node, endNode);

        nodeID = INVALID_ITEM_ID;
    }
    
    template <typename T, typename StorageType>
    void IDMapBase<T, StorageType>::FreeItems(ItemIterator first, ItemIterator last)
    {
        // add node and all children to free index list
        std::for_each(node, endNode, [this](Item& item) { const uint16_t index = IDMAP_INDEX_MASK(item.mID); mFreeIndirectionIndices.push_back(index); });

        mItems.erase(node, endNode);

        nodeID = INVALID_ITEM_ID;
    }

    template <typename T, typename StorageType>
    void IDMapBase<T, StorageType>::Insert(const ItemID itemID)
    {
        
    }*/
}