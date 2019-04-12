#include "Plane.h"

namespace JonsEngine
{
    // xyzw = abcd
    Plane::Plane(const Vec4& planeCoefficients) :
        mNormal(glm::normalize(Vec3(planeCoefficients.x, planeCoefficients.y, planeCoefficients.z))),
        mDistance(-planeCoefficients.w / (glm::sqrt(planeCoefficients.x * planeCoefficients.x + planeCoefficients.y * planeCoefficients.y + planeCoefficients.z * planeCoefficients.z)))
    {
    }

    Plane::Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3) : mNormal(glm::normalize(glm::cross(point2 - point1, point3 - point1))), mDistance(glm::dot(mNormal, point1))
    {
        assert(mNormal != Vec3(0.0f));
    }

    Plane::~Plane()
    {
    }


    const Vec3& Plane::GetNormal() const
    {
        return mNormal;
    }

    float Plane::GetDistance() const
    {
        return mDistance;
    }
}