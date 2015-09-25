#pragma once

#include "include/Core/Types.h"

#include <array>
#include <vector>

namespace JonsEngine
{
    class Plane
    {
    public:
        Plane(const Vec4& planeCoefficients);
        Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3);
        ~Plane();

        const Vec3& GetNormal() const;
        float GetDistance() const;


    private:
        Vec3 mNormal;
        float mDistance;
    };

    typedef std::vector<Plane> KDOP;
}