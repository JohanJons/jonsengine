#pragma once

#include "Core/Containers/IDMapBase.hpp"
#include "Core/Types.h"

#include <vector>
#include <algorithm>
#include <assert.h>

namespace JonsEngine
{
    // Stable map-like container with contigous memory using IDs for keys
    // Useful for when both instant lookup of keys and fast container iteration is needed

    // Internal storage item
    template <typename T>
    struct IDMapItem
    {
        ItemID mID;
        T mItem;

        template <typename... Arguments>
        IDMapItem(const ItemID id, Arguments&&... args);
    };

    template<typename T>
    class IDMap : public IDMapBase<T, IDMapItem<T>>
    {
    public:
        template <typename... Arguments>
        ItemID Insert( Arguments&&... args );
        void Erase( ItemID& id );

        T& GetItem( const ItemID id );
        const T& GetItem( const ItemID id ) const;
		ItemID GetID( const T& item ) const;

        void Clear();
    };


	//
	// IDMapItem
	//

	template <typename T>
	template <typename... Arguments>
    IDMapItem<T>::IDMapItem(const ItemID id, Arguments&&... args) : mID(id), mItem{std::forward<Arguments>(args)...}
	{
	}



	//
	// IDMap
	//

    template <typename T>
	template <typename... Arguments>
	typename IDMap<T>::ItemID IDMap<T>::Insert( Arguments&&... args )
	{
        const uint32_t newChildIndex = static_cast<uint32_t>( mItems.size() );

		if (!mFreeIndirectionIndices.empty())
		{
            const uint16_t freeIndirectionIndex = mFreeIndirectionIndices.back();
            mFreeIndirectionIndices.pop_back();

            ItemID& indirectionID = mIndirectionLayer.at(freeIndirectionIndex);
            const uint32_t version = static_cast<uint32_t>(IDMAP_VERSION_MASK(indirectionID) + 1);
            indirectionID = newChildIndex | (version << 16);
            
            const ItemID publicID = freeIndirectionIndex | (version << 16);
            mItems.emplace_back(publicID, std::forward<Arguments>(args)...);
            
            assert( publicID != INVALID_ITEM_ID );

            return publicID;
		}
		else
		{
            const uint32_t version = 1;
            const uint32_t indirectionIndex = static_cast<uint32_t>( mItems.size() );
            const ItemID indirectionID = newChildIndex | (version << 16);
            mIndirectionLayer.emplace_back(indirectionID);

            const ItemID publicID = indirectionIndex | (version << 16);
            mItems.emplace_back(publicID, std::forward<Arguments>(args)...);
            
            assert( publicID != INVALID_ITEM_ID );

            return publicID;
		}
	}

	template <typename T>
    void IDMap<T>::Erase( ItemID& id )
	{
        ItemIterator item = GetItemIter(id);

        // decrement indirection layer indexes
        std::for_each(item, mItems.end(), [this](Item& item) { ItemID& indirID = GetIndirectionID(item.mID); --indirID; });

        mItems.erase(item);
        
        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(id);
        mFreeIndirectionIndices.push_back(indirectionIndex);
	}

	template <typename T>
	T& IDMap<T>::GetItem( const ItemID id )
	{
		return GetItemIter( id )->mItem;
	}

    template <typename T>
    const T& IDMap<T>::GetItem( const ItemID id ) const
    {
        return GetItemIter( id )->mItem;
    }

	template <typename T>
	ItemID IDMap<T>::GetID( const T& item ) const
	{
		auto iter = std::find_if( mItems.cbegin(), mItems.cend(), [ &item ]( const IDMapItem<T>& itemBase ) { return &item == &itemBase.mItem; } );
		if ( iter == mItems.cend() )
			return INVALID_ITEM_ID;

		return iter->mID;
	}

	template <typename T>
	void IDMap<T>::Clear()
	{
		mItems.clear();
		mIndirectionLayer.clear();
        mFreeIndirectionIndices.clear();
	}
}