#include "include/Scene/Camera.h"

#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    static const Vec3 gAxisX(1.0f, 0.0f, 0.0f);
    static const Vec3 gAxisY(0.0f, 1.0f, 0.0f);
    static const Vec3 gAxisZ(0.0f, 0.0f, 1.0f);


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
        return Vec3(glm::inverse(Orientation()) * Vec4(-gAxisZ, 0.0f));
    }

    Vec3 Camera::Right() const
    {
        return Vec3(glm::inverse(Orientation()) * Vec4(gAxisX, 0.0f));
    }

    Vec3 Camera::Position() const
    {
        return mTranslation;
    }

    Mat4 Camera::Orientation() const
    {
        Quaternion rotation = glm::angleAxis(glm::radians(mVerticalAngle), gAxisX);
        rotation = rotation * glm::angleAxis(glm::radians(mHorizontalAngle), gAxisY);

        return glm::toMat4(rotation);
    }

       
    Mat4 Camera::GetCameraTransform() const
    {
        return glm::translate(gIdentityMatrix, -mTranslation) * Orientation();
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