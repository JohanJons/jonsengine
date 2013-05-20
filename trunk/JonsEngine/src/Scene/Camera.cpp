#include "include/Scene/Camera.h"

#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    Camera::Camera() : mTranslation(0.0f), mHorizontalAngle(0.0f), mVerticalAngle(0.0f)
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
        mTranslation += translateVec;
    }
        
    void Camera::RotateCamera(const float offsetHorizontalAngle, const float offsetVerticalAngle)
    {
        mHorizontalAngle += offsetHorizontalAngle;
        mHorizontalAngle = fmod(mHorizontalAngle, 360.0f);

        mVerticalAngle += offsetVerticalAngle;
        mVerticalAngle = fmod(mVerticalAngle, 360.0f);
    }


    Vec3 Camera::Forward() const
    {
        Vec4 forward = Inverse(Orientation()) * Vec4(0.0f, 0.0f, 1.0f, 0.0f);

        return Vec3(forward);
    }

    Vec3 Camera::Right() const
    {
        Vec4 right = Inverse(Orientation()) * Vec4(-1.0f, 0.0f, 0.0f, 0.0f);

        return Vec3(right);
    }

    Mat4 Camera::Orientation() const
    {
        Quaternion rotation;
        rotation = AngleAxisToQuaternion(mVerticalAngle, Vec3(1.0f, 0.0f, 0.0f));
        rotation = rotation * AngleAxisToQuaternion(mHorizontalAngle, Vec3(0.0f, 1.0f, 0.0f));

        return QuaternionToMat4(rotation);
    }

       
    Mat4 Camera::GetCameraTransform() const
    {
        Mat4 viewMatrix(1.0f);
        viewMatrix = Orientation() * Translate(viewMatrix, mTranslation);

        return viewMatrix;
    }
}