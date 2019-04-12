#pragma once

#include "DX11Mesh.h"
#include "DX11ConstantBuffer.hpp"
#include "DX11Utils.h"
#include "DX11Shadowmap.h"
#include "RenderQueue.h"
#include "Types.h"
#include "Platform.h"

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

        DX11PointLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass, const RenderSettings::ShadowResolution shadowmapRes, const uint32_t windowWidth, const uint32_t windowHeight);
        ~DX11PointLightPass();

        void Render(const RenderablePointLight& pointLight, const RenderData& renderData, const Mat4& camViewMatrix, const Mat4& camViewProjMatrix);


    private:
        struct PointLightCBuffer
        {
            Vec4 mLightColor;
            Vec4 mLightPosition;
            Vec2 mWindowSize;
            float mLightIntensity;
            float mZNear;
            float mMaxDistance;
            float __padding;


            PointLightCBuffer(const Vec4& lightColor, const Vec4& lightPosition, const Vec2& windowSize, const float lightIntensity, const float zNear, const float maxDistance) :
                mLightColor(lightColor), mLightPosition(lightPosition), mWindowSize(windowSize), mLightIntensity(lightIntensity), mMaxDistance(maxDistance), mZNear(zNear)
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
