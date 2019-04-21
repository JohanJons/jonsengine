#pragma once

#include "Core/Containers/IDMap.hpp"
#include "Core/Utils/NonCopyable.h"

namespace JonsEngine
{
    // TODO: make NonCopyable if VS2015 >= due to noexcept not being present in VS2013 which makes moveconstructors and containers not play nice
    template <typename T>
    class IDMapPointer : NonCopyable
    {
    public:
        template <typename... Arguments>
        IDMapPointer(typename IDMap<T>& storage, Arguments&&... args) : mStorage(storage), mID(storage.Insert(args...))
        {
        }

        IDMapPointer(IDMapPointer&& other) throw() : mStorage(std::move(other.mStorage)), mID(std::move(other.mID))
        {
            other.mID = IDMap<T>::INVALID_ITEM_ID;
        }

        ~IDMapPointer()
        {
            if (mID != IDMap<T>::INVALID_ITEM_ID)
                mStorage.Erase(mID);
        }


        T& Get()
        {
            return mStorage.GetItem(mID);
        }

        const T& Get() const
        {
            return mStorage.GetItem(mID);
        }

        typename IDMap<T>::ItemID GetID() const
        {
            return mID;
        }


    private:
        typename IDMap<T>& mStorage;
        typename IDMap<T>::ItemID mID;
    };
}