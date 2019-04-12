#pragma once

#include "Types.h"
#include "Plane.h"

#include <array>
#include <vector>

namespace JonsEngine
{
    typedef std::array<Plane, 6> FrustumPlanes;
    typedef std::array<Vec4, 8> FrustumCorners;

    enum FrustumCorner
    {
        NearBottomLeft = 0,
        NearTopLeft,
        NearTopRight,
        NearBottomRight,
        FarBottomLeft,
        FarTopLeft,
        FarTopRight,
        FarBottomRight
    };

    enum FrustumPlane
    {
        Left = 0,
        Right,
        Top,
        Bottom,
        Near,
        Far
    };


    FrustumCorners GetFrustumCorners(const Mat4& viewFrustum);
    FrustumPlanes GetFrustumPlanes(const Mat4& viewFrustum);

    const std::array<FrustumPlane, 4>& GetNeighbouringPlanes(const FrustumPlane plane);
    const std::pair<FrustumCorner, FrustumCorner>& GetCornersOfPlanes(const FrustumPlane plane1, const FrustumPlane plane2);
}