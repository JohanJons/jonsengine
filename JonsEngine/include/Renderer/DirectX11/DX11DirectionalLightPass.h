#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Shadowmap.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Frustum.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/EngineSettings.h"
#include "include/Core/Platform/Directx11.h"

#include <array>
#include <vector>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;
    class DX11VertexTransformPass;
	struct RenderableDirLight;

    class DX11DirectionalLightPass
    {
    public:
        const static uint32_t NUM_SHADOWMAP_CASCADES = 4;

        DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const EngineSettings::ShadowResolution shadowmapRes,
            const EngineSettings::ShadowReadbackLatency readbackLatency, const uint32_t windowWidth, const uint32_t windowHeight);
        ~DX11DirectionalLightPass();

        void Render(const RenderableDirLight& directionalLight, const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage, const EngineSettings::ShadowFiltering shadowFiltering, const float degreesFOV, const Mat4& cameraViewMatrix, const Mat4& invCameraProjMatrix);


    private:
        struct DirectionalLightCBuffer
        {
            std::array<Mat4, NUM_SHADOWMAP_CASCADES> mSplitVPMatrices;
            Mat4 mInvProjMatrix;
            std::array<float, NUM_SHADOWMAP_CASCADES> mSplitDistances;
            Vec4 mLightColor;
            Vec4 mLightDirection;
            Vec2 mWindowSize;
            float mShadowmapSize;
            float __padding;


            DirectionalLightCBuffer(const std::array<Mat4, NUM_SHADOWMAP_CASCADES>& splitMatrices, const Mat4& invProjMatrix, const std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances, const Vec4& lightColor, const Vec4& lightDir, const Vec2& windowSize, const float shadowmapSize) :
                mSplitVPMatrices(splitMatrices), mInvProjMatrix(invProjMatrix), mSplitDistances(splitDistances), mLightColor(lightColor), mLightDirection(lightDir), mWindowSize(windowSize), mShadowmapSize(shadowmapSize)
            {
            }
        };

        void BindShadingPixelShader(const EngineSettings::ShadowFiltering shadowFiltering);


        const Vec2 mWindowSize;
        const float mAspectRatio;

        ID3D11DeviceContextPtr mContext;

        ID3D11PixelShaderPtr mPCF2x2Shader;
        ID3D11PixelShaderPtr mPCF3x3Shader;
        ID3D11PixelShaderPtr mPCF5x5Shader;
        ID3D11PixelShaderPtr mPCF7x7Shader;

        ID3D11RasterizerStatePtr mRSDepthClamp;
        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<DirectionalLightCBuffer> mDirLightCBuffer;
    };
}
