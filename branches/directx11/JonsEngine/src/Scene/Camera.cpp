#include "include/Scene/Camera.h"

#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    Camera::Camera() : mTranslation(0.0f, 0.0f, 0.0f), mHorizontalAngle(0.0f), mVerticalAngle(0.0f)
    {
    }
        
    Camera::~Camera()
    {
    }


    void Camera::SetPosition(const Vec3& position)
    {
        mTranslation = position;
    }

    void Camera::TranslateCamera(const Vec3& translateVec)
    {
        mTranslation = Add(mTranslation, translateVec);
    }
        
    void Camera::RotateCamera(const float offsetHorizontalAngle, const float offsetVerticalAngle)
    {
        mHorizontalAngle += offsetHorizontalAngle;
        mHorizontalAngle = std::fmod(mHorizontalAngle, 360.0f);

        mVerticalAngle += offsetVerticalAngle;
        mVerticalAngle = std::fmod(mVerticalAngle, 360.0f);
    }


    Vec3 Camera::Forward() const
    {
        Vec4 temp = Multiply(Vec4(0.0f, 0.0f, -1.0f, 0.0f), Inverse(Orientation()));

        return Vec3(temp.x, temp.y, temp.z);
    }

    Vec3 Camera::Right() const
    {
        Vec4 temp = Multiply(Vec4(1.0f, 0.0f, 0.0f, 0.0f), Inverse(Orientation()));

        return Vec3(temp.x, temp.y, temp.z);
    }

    Vec3 Camera::Position() const
    {
        return mTranslation;
    }

    Mat4 Camera::Orientation() const
    {
        Quaternion rotation = GetIdentityQuaternion();

        rotation = RotateAxis(rotation, Vec3(1.0f, 0.0f, 0.0f), mVerticalAngle);
        rotation = RotateAxis(rotation, Vec3(0.0f, 1.0f, 0.0f), mHorizontalAngle);

        return RotationMatrix(rotation);
    }

       
    Mat4 Camera::GetCameraTransform() const
    {
        return Multiply(Translate(GetIdentityMatrix(), mTranslation), Orientation());
    }
}