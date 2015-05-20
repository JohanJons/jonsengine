#pragma once

#include "include/Core/Types.h"

namespace JonsEngine
{
    template <uint32_t MAX_KDOP_PLANES>
    class KDOP
    {
    public:
        inline KDOP() : mNumPlanes(0)
        {
        }


        inline Plane& operator[] (const uint32_t index)
        {
            assert(index < mNumPlanes);

            return mPlanes[index];
        }

        inline const Plane& operator[] (const uint32_t index) const
        {
            assert(index < mNumPlanes);

            return mPlanes[index];
        }


        inline uint32_t TotalPlanes() const
        {
            return mNumPlanes;
        }

        inline void AddPlane(const Plane& plane)
        {
            assert(mNumPlanes < MAX_KDOP_PLANES);

            mPlanes[++mNumPlanes] = plane;
        }

        inline void Reset()
        {
            mNumPlanes = 0;
        }


    private:
        std::array<Plane, MAX_KDOP_PLANES> mPlanes;
        uint32_t mNumPlanes;
    };
}