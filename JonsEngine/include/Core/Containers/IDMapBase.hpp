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
        typedef typename std::vector<Item>::const_iterator ConstItemIterator;

        // this is admittedly rather ugly...
        typedef ItemID ItemID;
        const static ItemID INVALID_ITEM_ID = 0;

        // TODO: maybe extend to adhere to RandomAccessIterator
        // TODO: some of these template parameters are probably redundant. Should be possible to trim down.
        template <typename ValueRefType, typename InternalIter>
        class BaseIterator
        {
        public:
            BaseIterator(typename const InternalIter& iter);

            bool operator!=(const BaseIterator& iter) const;

            BaseIterator& operator++();
            BaseIterator operator++(int) const;

            void operator+=(const size_t offset);
            BaseIterator operator+ (const size_t offset) const;

            ValueRefType operator*() const;


        private:
            InternalIter mIterator;
        };

        typedef BaseIterator<T&, ItemIterator> iterator;
        typedef BaseIterator<const T&, ConstItemIterator> const_iterator;


        iterator begin();
        iterator end();

        const_iterator cbegin() const;
        const_iterator cend() const;


    protected:
        ItemIterator GetItemIter( const ItemID nodeID );
        ConstItemIterator GetItemIter( const ItemID nodeID ) const;
        ItemID& GetIndirectionID( const ItemID publicID );
        const ItemID& GetIndirectionID( const ItemID publicID ) const;


        std::vector<Item> mItems;
        std::vector<ItemID> mIndirectionLayer;
        std::vector<uint16_t> mFreeIndirectionIndices;
    };
    
    #define IDMAP_INDEX_MASK(id) (id & 0xFFFFFFFF)
    #define IDMAP_VERSION_MASK(id) ((id >> 16) & 0xFFFFFFFF)


    //
    // IDMapBase::BaseIterator
    //

    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::BaseIterator(typename const InternalIter& iter) : mIterator(iter)
    {
    }

    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    bool IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::operator!=(const BaseIterator<ValueRefType, InternalIter>& iter) const
    {
        return mIterator != iter.mIterator;
    }

    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    typename IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>& IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::operator++()
    {
        ++mIterator;

        return *this;
    }

    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    typename IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter> IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::operator++(int) const
    {
        BaseIterator<InternalIter> old(++(*this));

        return old;
    }


    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    void IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::operator+=(const size_t offset)
    {
        mIterator += offset;
    }

    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    typename IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter> IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::operator+ (const size_t offset) const
    {
        return mIterator + offset;
    }


    template <typename T, typename StorageType>
    template <typename ValueRefType, typename InternalIter>
    ValueRefType IDMapBase<T, StorageType>::BaseIterator<ValueRefType, InternalIter>::operator*() const
    {
        return mIterator->mItem;
    }


    //
    // IDMapBase
    //
    
    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::ItemIterator IDMapBase<T, StorageType>::GetItemIter( const ItemID nodeID )
    {
        const ItemID& itemID = GetIndirectionID(nodeID);
        const uint16_t itemIndex = IDMAP_INDEX_MASK(itemID);

        assert(itemIndex <= mItems.size());

        return mItems.begin() + itemIndex;
    }
    
    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::ItemID& IDMapBase<T, StorageType>::GetIndirectionID( const ItemID publicID )
    {
        assert(publicID != INVALID_ITEM_ID);

        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(publicID);
        const uint16_t version = IDMAP_VERSION_MASK(publicID);

        ItemID& indirID = mIndirectionLayer.at(indirectionIndex);

        assert(version == IDMAP_VERSION_MASK(indirID));

        return indirID;
    }


    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::ConstItemIterator IDMapBase<T, StorageType>::GetItemIter( const ItemID nodeID ) const
    {
        const ItemID& itemID = GetIndirectionID(nodeID);
        const uint16_t itemIndex = IDMAP_INDEX_MASK(itemID);

        assert(itemIndex <= mItems.size());

        return mItems.cbegin() + itemIndex;
    }

    template <typename T, typename StorageType>
    const typename IDMapBase<T, StorageType>::ItemID& IDMapBase<T, StorageType>::GetIndirectionID( const ItemID publicID ) const
    {
        assert(publicID != INVALID_ITEM_ID);

        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(publicID);
        const uint16_t version = IDMAP_VERSION_MASK(publicID);

        const ItemID& indirID = mIndirectionLayer.at(indirectionIndex);

        assert(version == IDMAP_VERSION_MASK(indirID));

        return indirID;
    }


    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::iterator IDMapBase<T, StorageType>::begin()
    {
        return iterator(mItems.begin());
    }

    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::iterator IDMapBase<T, StorageType>::end()
    {
        return iterator(mItems.end());
    }


    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::const_iterator IDMapBase<T, StorageType>::cbegin() const
    {
        return const_iterator(mItems.cbegin());
    }

    template <typename T, typename StorageType>
    typename IDMapBase<T, StorageType>::const_iterator IDMapBase<T, StorageType>::cend() const
    {
        return const_iterator(mItems.cend());
    }
}