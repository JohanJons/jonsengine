#pragma once

#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Shadowmap.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <d3d11.h>
#include <vector>
#include <array>

namespace JonsEngine
{
    class DX11VertexTransformPass;
	struct RenderablePointLight;

    class DX11PointLightPass
    {
    public:
        enum POINT_LIGHT_DIR
        {
            POINT_LIGHT_DIR_POS_X = 0,
            POINT_LIGHT_DIR_NEG_X,
            POINT_LIGHT_DIR_POS_Y,
            POINT_LIGHT_DIR_NEG_Y,
            POINT_LIGHT_DIR_NEG_Z,
            POINT_LIGHT_DIR_POS_Z,
            POINT_LIGHT_DIR_COUNT
        };

        DX11PointLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass, const EngineSettings::ShadowResolution shadowmapRes, const uint32_t windowWidth, const uint32_t windowHeight);
        ~DX11PointLightPass();

        void Render(const RenderablePointLight& pointLight, const IDMap<Mat4>& localTransformStorage, const IDMap<Mat4>& worldTransformStorage, const Mat4& camViewMatrix, const Mat4& camViewProjMatrix, const Mat4& invCameraProjMatrix);

        void BindForShading();


    private:
        struct PointLightCBuffer
        {
            Mat4 mInvProjMatrix;
            Vec4 mLightColor;
            Vec4 mLightPosition;
            Vec2 mWindowSize;
            float mLightIntensity;
            float mZNear;
            float mMaxDistance;
            float __padding;


            PointLightCBuffer(const Mat4& invProjMatrix, const Vec4& lightColor, const Vec4& lightPosition, const Vec2& windowSize, const float lightIntensity, const float zNear, const float maxDistance) :
                mInvProjMatrix(invProjMatrix), mLightColor(lightColor), mLightPosition(lightPosition), mWindowSize(windowSize), mLightIntensity(lightIntensity), mMaxDistance(maxDistance), mZNear(zNear)
            {
            }
        };


        const Vec2 mWindowSize;

        ID3D11DeviceContextPtr mContext;

        ID3D11PixelShaderPtr mPixelShader;
        ID3D11DepthStencilStatePtr mDSSStencilPass;
        ID3D11DepthStencilStatePtr mDSSShadingPass;
        ID3D11RasterizerStatePtr mRSCullFront;
        ID3D11RasterizerStatePtr mRSNoCulling;

        DX11Mesh mSphereMesh;
        DX11VertexTransformPass& mVertexTransformPass;
        DX11Shadowmap mShadowmap;
        DX11ConstantBuffer<PointLightCBuffer> mPointLightCBuffer;
    };
}
