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

        typedef typename std::vector<Item> ItemContainer;
        typedef typename ItemContainer::iterator ItemIterator;

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
}