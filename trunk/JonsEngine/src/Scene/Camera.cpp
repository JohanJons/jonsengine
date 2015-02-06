#include "include/Scene/Camera.h"

#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    Camera::Camera() : mTranslation(0.0f), mHorizontalAngle(0.0f), mVerticalAngle(0.0f), mFOV(70.0f)
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
        mHorizontalAngle = std::fmod(mHorizontalAngle, 360.0f);

        mVerticalAngle += offsetVerticalAngle;
        mVerticalAngle = std::fmod(mVerticalAngle, 360.0f);
    }


    Vec3 Camera::Forward() const
    {
        return Vec3(glm::inverse(Orientation()) * Vec4(0.0f, 0.0f, -1.0f, 0.0f));
    }

    Vec3 Camera::Right() const
    {
        return Vec3(glm::inverse(Orientation()) * Vec4(1.0f, 0.0f, 0.0f, 0.0f));
    }

    Vec3 Camera::Position() const
    {
        return mTranslation;
    }

    Mat4 Camera::Orientation() const
    {
        Quaternion rotation;
        rotation = glm::angleAxis(glm::radians(mVerticalAngle), Vec3(1.0f, 0.0f, 0.0f));
        rotation = rotation * glm::angleAxis(glm::radians(mHorizontalAngle), Vec3(0.0f, 1.0f, 0.0f));

        return glm::toMat4(rotation);
    }

       
    Mat4 Camera::GetCameraTransform() const
    {
        return Orientation() * glm::translate(Mat4(1.0f), -mTranslation);    // TODO: gimbal lock?
    }


	void Camera::SetFOV(const float fov)
	{
		mFOV = fov;
	}
		
	float Camera::GetFOV() const
	{
		return mFOV;
	}
}