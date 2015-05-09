#include "include/Core/Math/CameraFrustrum.h"
#include "include/Core/Math/Math.h"

#include <limits>

namespace JonsEngine
{
    CameraFrustrum::CameraFrustrum(const Mat4& camViewProjMatrix) : 
        mCorners({{ Vec4(1.0f, -1.0f, 0.0f, 1.0f), Vec4(1.0f, 1.0f, 0.0f, 1.0f), Vec4(-1.0f, 1.0f, 0.0f, 1.0f), Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
                    Vec4(1.0f, -1.0f, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, -1.0f, 1.0f, 1.0f) }})
    {
        Vec4 min(-std::numeric_limits<float>::max()), max(std::numeric_limits<float>::max());
        const Mat4 invVP = glm::inverse(camViewProjMatrix);

        for (Vec4& corner : mCorners)
        {
            corner = invVP * corner;
            corner /= corner.w;

            min = MinVal(min, corner);
            max = MaxVal(max, corner);
        }

        mAABB = AABB(Vec3(0.5f * (min + max)), Vec3(0.5f * (max - min)));
    }

    CameraFrustrum::~CameraFrustrum()
    {
    }
}