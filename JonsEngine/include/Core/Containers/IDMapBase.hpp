#pragma once

#include "include/Core/Types.h"

#include <vector>
#include <assert.h>

namespace JonsEngine
{
    // base class for ID-based map containers
    template<typename T/*, typename StorageType*/>
    class IDMapBase
    {
    public:
        typedef uint32_t ItemID;

        //typedef typename std::vector<StorageType> ItemContainer;
        //typedef typename ItemContainer::iterator ItemIterator;

        const static ItemID INVALID_ITEM_ID = 0;


        /*class iterator {
        public:
            iterator(typename const ItemIterator& iter);

            bool operator!=(const iterator& iter) const;
            iterator& operator++();
            iterator operator++(int);
            T& operator*();
            const T& operator*() const;


        private:
            typename ItemIterator mIterator;
        };*/
    };
    
    #define IDMAP_INDEX_MASK(id) (id & 0xFFFFFFFF)
    #define IDMAP_VERSION_MASK(id) ((id >> 16) & 0xFFFFFFFF)
}