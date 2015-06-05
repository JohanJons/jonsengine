#pragma once

#include "include/Core/Types.h"
#include "include/Core/Math/Plane.h"
#include "include/Core/Math/Frustum.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Containers/RangedIterator.hpp"

#include <string>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class DirectionalLight
    {
    public:
        DirectionalLight(const std::string& name, const uint32_t numShadowmapCascades);

        bool operator==(const DirectionalLight& light);
        bool operator==(const std::string& lightName);

        void SetCascadeSplitLambda(const float lambda);
        void UpdateCascadesBoundingVolume(const Mat4& viewMatrix, const float degreesFOV, const float aspectRatio, const float minDepth, const float maxDepth);
        
        ConstRangedIterator<KDOP> GetBoundingVolume(const uint32_t cascadeIndex) const;
        void GetSplitDistance(const uint32_t cascadeIndex, float& nearZ, float& farZ) const;


        const std::string mName;
        const uint32_t mNumShadowmapCascades;

        Vec4 mLightColor;
        Vec3 mLightDirection;


	private:
        void UpdateSplitDistances(const float minDepth, const float maxDepth);
        void UpdateKDOP(const FrustumPlanes& frustumPlanes, const FrustumCorners& frustumCorners);


		const size_t mHashedID;

        KDOP mKDOP;
        std::vector<size_t> mCascadeKDOPRange;
        std::vector<float> mSplitDistances;
        float mCascadeSplitLambda;
    };

	typedef std::unique_ptr<DirectionalLight, std::function<void(DirectionalLight*)>> DirectionalLightPtr;
}