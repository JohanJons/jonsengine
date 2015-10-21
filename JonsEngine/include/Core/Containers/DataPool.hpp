#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

namespace JonsEngine
{
    // TODO: functionality like memory allocators, useful statistics...
    template <typename T>
    class DataPool
    {
    public:
        IDMap<T>& GetStorage()
        {
            return mStorage;
        }

        const IDMap<T>& GetStorage() const
        {
            return mStorage;
        }


    private:
        IDMap<T> mStorage;
    };
}