#pragma once

#include "include/Core/Math/Plane.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    template <uint32_t MAX_KDOP_PLANES>
    class KDOP
    {
    public:
        KDOP();
        ~KDOP();

        Plane& operator[] (const uint32_t index);
        const Plane& operator[] (const uint32_t index) const;

        uint32_t TotalPlanes() const;
        void AddPlane(const Plane& plane);
        void Reset();


    private:
        std::array<Plane, MAX_KDOP_PLANES> mPlanes;
        uint32_t mNumPlanes;
    };
}

#include "include/Core/Math/KDOP.hpp"