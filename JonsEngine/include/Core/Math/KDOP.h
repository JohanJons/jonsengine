#pragma once

#include "include/Core/Math/Plane.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    template <uint32_t MAX_KDOP_PLANES>
    class KDOP
    {
    public:
        typedef std::array<Plane, MAX_KDOP_PLANES> array_container;
        typedef typename array_container::iterator iterator;


        KDOP();
        ~KDOP();

        Plane& operator[] (const uint32_t index);
        const Plane& operator[] (const uint32_t index) const;

        iterator begin();
        iterator end();

        uint32_t TotalPlanes() const;
        void AddPlane(const Plane& plane);
        void Reset();


    private:
        array_container mPlanes;
        uint32_t mNumPlanes;
    };
}

#include "include/Core/Math/KDOP.hpp"