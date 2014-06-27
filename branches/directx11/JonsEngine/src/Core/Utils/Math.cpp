#include "include/Core/Utils/Math.h"

using namespace DirectX;

namespace JonsEngine
{
    Vec2 Add(const Vec2& vec1, const Vec2& vec2)
    {
        Vec2 ret;

        XMStoreFloat2(&ret, XMLoadFloat2(&vec1) + XMLoadFloat2(&vec2));

        return ret;
    }

    Vec3 Add(const Vec3& vec1, const Vec3& vec2)
    {
        Vec3 ret;

        XMStoreFloat3(&ret, XMLoadFloat3(&vec1) + XMLoadFloat3(&vec2));

        return ret;
    }

    Mat3 Add(const Mat3& mat1, const Mat3& mat2)
    {
        Mat3 ret;

        XMStoreFloat3x3(&ret, XMLoadFloat3x3(&mat1) + XMLoadFloat3x3(&mat2));

        return ret;
    }

    Mat4 Add(const Mat4& mat1, const Mat4& mat2)
    {
        Mat4 ret;

        XMStoreFloat4x4(&ret, XMLoadFloat4x4(&mat1) + XMLoadFloat4x4(&mat2));

        return ret;
    }


    Vec2 Multiply(const Vec2& vec1, const Vec2& vec2)
    {
        Vec2 ret;

        XMStoreFloat2(&ret, XMLoadFloat2(&vec1) * XMLoadFloat2(&vec2));

        return ret;
    }

    Vec3 Multiply(const Vec3& vec1, const Vec3& vec2)
    {
        Vec3 ret;

        XMStoreFloat3(&ret, XMLoadFloat3(&vec1) * XMLoadFloat3(&vec2));

        return ret;
    }

    Vec3 Multiply(const Vec3& vec, const float magnitude)
    {
        Vec3 ret;

        XMStoreFloat3(&ret, XMLoadFloat3(&vec) * magnitude);

        return ret;
    }

    Vec4 Multiply(const Vec4& vec, const Mat4& mat)
    {
        Vec4 ret;

        XMStoreFloat4(&ret, XMVector4Transform(XMLoadFloat4(&vec), XMLoadFloat4x4(&mat)));

        return ret;
    }

    Mat3 Multiply(const Mat3& mat1, const Mat3& mat2)
    {
        Mat3 ret;

        XMStoreFloat3x3(&ret, XMLoadFloat3x3(&mat1) * XMLoadFloat3x3(&mat2));

        return ret;
    }

    Mat4 Multiply(const Mat4& mat1, const Mat4& mat2)
    {
        Mat4 ret;

        XMStoreFloat4x4(&ret, XMLoadFloat4x4(&mat1) * XMLoadFloat4x4(&mat2));

        return ret;
    }

    Quaternion Multiply(const Quaternion& quat1, const Quaternion& quat2)
    {
        Quaternion ret;

        XMStoreFloat4(&ret, XMLoadFloat4(&quat1) * XMLoadFloat4(&quat2));

        return ret;
    }


    Vec3 Negate(const Vec3& vec)
    {
        Vec3 ret;

        XMStoreFloat3(&ret, XMVectorNegate(XMLoadFloat3(&vec)));

        return ret;
    }

    Mat4 Inverse(const Mat4& mat)
    {
        Mat4 ret;

        XMStoreFloat4x4(&ret, XMMatrixInverse(nullptr, XMLoadFloat4x4(&mat)));

        return ret;
    }

    Quaternion RotateAxis(const Quaternion& quaternion, const Vec3& rotateVec, const float angle)
    {
        Quaternion ret;

        XMVECTOR sourceOrientation = XMLoadFloat4(&quaternion), rotationVector = XMLoadFloat3(&rotateVec);
        XMVECTOR newOrientation = XMQuaternionRotationAxis(rotationVector, angle);
        XMStoreFloat4(&ret, sourceOrientation * newOrientation);

        return ret;
    }

    Mat4 Perspective(const float angle, const float aspectRatio, const float nearZ, const float farZ)
    {
        Mat4 ret;

        XMMATRIX temp = XMMatrixPerspectiveFovLH(angle, aspectRatio, nearZ, farZ);
        XMStoreFloat4x4(&ret, temp);

        return ret;
    }


    Mat4 Translate(const Mat4& mat, const Vec3& offset)
    {
        Mat4 ret;

        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&offset));
        XMStoreFloat4x4(&ret, XMLoadFloat4x4(&mat) * translationMatrix);

        return ret;
    }

    Mat4 Scale(const Mat4& mat, const Vec3& scale)
    {
        Mat4 ret;

        XMMATRIX scalingMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
        XMStoreFloat4x4(&ret, XMLoadFloat4x4(&mat) * scalingMatrix);

        return ret;
    }

    Mat4 RotationMatrix(const Quaternion& quaternion)
    {
        Mat4 ret;

        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion));
        XMStoreFloat4x4(&ret, rotationMatrix);

        return ret;
    }

    Mat4 GetIdentityMatrix()
    {
        Mat4 ret;

        XMStoreFloat4x4(&ret, XMMatrixIdentity());

        return ret;
    }

    Quaternion GetIdentityQuaternion()
    {
        Quaternion ret;

        XMStoreFloat4(&ret, XMQuaternionIdentity());

        return ret;
    }
}