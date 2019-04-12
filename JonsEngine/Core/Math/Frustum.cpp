#include "Frustum.h"

namespace JonsEngine
{
    const static std::array<std::array<FrustumPlane, 4>, 6> gPlaneNeighbours =
    { 
        {{	// Left
            FrustumPlane::Top,
            FrustumPlane::Bottom,
            FrustumPlane::Near,
            FrustumPlane::Far,
        },
        {	// Right
            FrustumPlane::Top,
            FrustumPlane::Bottom,
            FrustumPlane::Near,
            FrustumPlane::Far
        },
        {	// Top
            FrustumPlane::Left,
            FrustumPlane::Right,
            FrustumPlane::Near,
            FrustumPlane::Far
        },
        {	// Bottom
            FrustumPlane::Left,
            FrustumPlane::Right,
            FrustumPlane::Near,
            FrustumPlane::Far
        },
        {	// Near
            FrustumPlane::Left,
            FrustumPlane::Right,
            FrustumPlane::Top,
            FrustumPlane::Bottom
        },
        {	// Far
            FrustumPlane::Left,
            FrustumPlane::Right,
            FrustumPlane::Top,
            FrustumPlane::Bottom
        }}
    };

    const static std::array<std::array<std::pair<FrustumCorner, FrustumCorner>, 6>, 6> gCornersOfPlaneNeighbours =
    {
        {{	// Left
            {{	// Left
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Right
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Top
                NearTopLeft, FarTopLeft,
            },
            {	// Bottom
                FarBottomLeft, NearBottomLeft,
            },
            {	// Near
                NearBottomLeft, NearTopLeft,
            },
            {	// Far
                FarTopLeft, FarBottomLeft,
            }},
        },
        {	// Right
            {{	// Left
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Right
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Top
                FarTopRight, NearTopRight,
            },
            {	// Bottom
                NearBottomRight, FarBottomRight,
            },
            {	// Near
                NearTopRight, NearBottomRight,
            },
            {	// Far
                FarBottomRight, FarTopRight,
            }},
        },
        {	// Top
            {{	// Left
                FarTopLeft, NearTopLeft,
            },
            {	// Right
                NearTopRight, FarTopRight,
            },
            {	// Top
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Bottom
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Near
                NearTopLeft, NearTopRight,
            },
            {	// Far
                FarTopRight, FarTopLeft,
            }},
        },
        {	// Bottom
            {{	// Left
                NearBottomLeft, FarBottomLeft,
            },
            {	// Right
                FarBottomRight, NearBottomRight,
            },
            {	// Top
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Bottom
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Near
                NearBottomRight, NearBottomLeft,
            },
            {	// Far
                FarBottomLeft, FarBottomRight,
            }},
        },
        {	// Near
            {{	// Left
                NearTopLeft, NearBottomLeft,
            },
            {	// Right
                NearBottomRight, NearTopRight,
            },
            {	// Top
                NearTopRight, NearTopLeft,
            },
            {	// Bottom
                NearBottomLeft, NearBottomRight,
            },
            {	// Near
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Far
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            }}
        },
        {	// Far
            {{	// Left
                FarBottomLeft, FarTopLeft,
            },
            {	// Right
                FarTopRight, FarBottomRight,
            },
            {	// Top
                FarTopLeft, FarTopRight,
            },
            {	// Bottom
                FarBottomRight, FarBottomLeft,
            },
            {	// Near
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            },
            {	// Far
                FarBottomLeft, FarBottomLeft,		// Invalid combination.
            }}
        }},
    };




    FrustumCorners GetFrustumCorners(const Mat4& cameraViewProjMatrix)
    {
        FrustumCorners ret = { Vec4(-1.0f, -1.0f, 0.0f, 1.0f),     // near bottom-left
            Vec4(-1.0f, 1.0f, 0.0f, 1.0f),      // near top-left
            Vec4(1.0f, 1.0f, 0.0f, 1.0f),       // near top-right
            Vec4(1.0f, -1.0f, 0.0f, 1.0f),      // near bottom-right
            Vec4(-1.0f, -1.0f, 1.0f, 1.0f),     // far bottom-left
            Vec4(-1.0f, 1.0f, 1.0f, 1.0f),      // far top-left
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),       // far top-right
            Vec4(1.0f, -1.0f, 1.0f, 1.0f), };   // far bottom-right

        const Mat4 invVP = glm::inverse(cameraViewProjMatrix);

        for (Vec4& corner : ret)
        {
            corner = invVP * corner;
            corner /= corner.w;
        }

        return ret;
    }

    FrustumPlanes GetFrustumPlanes(const Mat4& viewFrustum)
    {
        const Vec4 rowX(viewFrustum[0].x, viewFrustum[1].x, viewFrustum[2].x, viewFrustum[3].x);
        const Vec4 rowY(viewFrustum[0].y, viewFrustum[1].y, viewFrustum[2].y, viewFrustum[3].y);
        const Vec4 rowZ(viewFrustum[0].z, viewFrustum[1].z, viewFrustum[2].z, viewFrustum[3].z);
        const Vec4 rowW(viewFrustum[0].w, viewFrustum[1].w, viewFrustum[2].w, viewFrustum[3].w);

        return FrustumPlanes
        { {
                Plane(rowW + rowX),      // left
                Plane(rowW - rowX),      // right
                Plane(rowW - rowY),      // top
                Plane(rowW + rowY),      // bottom
                Plane(rowZ),             // near
                Plane(rowW - rowZ),      // far
            } };
    }


    const std::array<FrustumPlane, 4>& GetNeighbouringPlanes(const FrustumPlane plane)
    {
        return gPlaneNeighbours.at(plane);
    }

    const std::pair<FrustumCorner, FrustumCorner>& GetCornersOfPlanes(const FrustumPlane plane1, const FrustumPlane plane2)
    {
        return gCornersOfPlaneNeighbours.at(plane1).at(plane2);
    }
}