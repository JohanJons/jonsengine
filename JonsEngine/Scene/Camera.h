#pragma once

#include "Types.h"

namespace JonsEngine
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        void SetPosition(const Vec3& position);
        void TranslateCamera(const Vec3& translateVec);
        void RotateCamera(const float offsetHorizontalAngle, const float offsetVerticalAngle);

        Vec3 Forward() const;
        Vec3 Right() const;
        Vec3 Position() const;
        Mat4 Orientation() const;

        Mat4 GetCameraTransform() const;

        void SetFOV(const float fov);
		float GetFOV() const;


    public:
        Vec3 mTranslation;
        float mHorizontalAngle;
        float mVerticalAngle;
		float mFOV;
    };
}