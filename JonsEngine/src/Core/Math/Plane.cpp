#include "include/Core/Math/Plane.h"

namespace JonsEngine
{
    Plane::Plane() : mNormal(glm::normalize(1.0f)), mDistance(0.0f)
    {
    }

    // xyzw = abcd
    Plane::Plane(const Vec4& planeCoefficients) :
        mNormal(glm::normalize(Vec3(planeCoefficients.x, planeCoefficients.y, planeCoefficients.z))),
        mDistance(planeCoefficients.w / (glm::sqrt(glm::pow(mNormal.x, 2) + glm::pow(mNormal.y, 2) + glm::pow(mNormal.z, 2))))
    {
    }

    Plane::Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3) : mNormal(glm::normalize(glm::cross(point2 - point1, point3 - point1))), mDistance(glm::dot(mNormal, point1))
    {
        assert(mNormal != Vec3(0.0f));
    }

    Plane::~Plane()
    {
    }
}