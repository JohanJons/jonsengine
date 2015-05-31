#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Utils/NonCopyable.h"

namespace JonsEngine
{
    template <typename T>
    class IDMapPointer : NonCopyable
    {
    public:
        template <typename... Arguments>
        IDMapPointer(typename IDMap<T>& storage, Arguments&&... args) : mStorage(storage), mID(storage.AddItem(args...))
        {
        }

        ~IDMapPointer()
        {
            if (mID != IDMap<T>::INVALID_ITEM_ID)
                mStorage.MarkAsFree(mID);
        }

        IDMapPointer(IDMapPointer&& other) throw() : mStorage(std::move(other.mStorage)), mID(std::move(other.mID))
        {
            other.mID = IDMap<T>::INVALID_ITEM_ID;
        }


        T& operator*()
        {
            return mStorage.GetItem(mID);
        }
        const T& operator*() const
        {
            return mStorage.GetItem(mID);
        }


        T* operator->()
        {
            return &mStorage.GetItem(mID);
        }

        const T* operator->() const
        {
            return &mStorage.GetItem(mID);
        }


    private:
        typename IDMap<T>& mStorage;
        typename IDMap<T>::ItemID mID;
    };
}