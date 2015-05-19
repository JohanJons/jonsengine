#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/RenderQueue.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Pipeline.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF2X2Pixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF3X3Pixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF5X5Pixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPCF7X7Pixel.h"
#include "include/Core/Math/Math.h"

#include <array>

namespace JonsEngine
{
    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, -0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.5f, 0.5f, 0.0f, 1.0f);


    Mat4 CreateDirLightVPMatrix(const CameraFrustrum& cameraFrustrum, const Vec3& lightDir)
    {
        Mat4 lightViewMatrix = glm::lookAt(Vec3(0.0f), -glm::normalize(lightDir), Vec3(0.0f, -1.0f, 0.0f));

        Vec4 transf = lightViewMatrix * cameraFrustrum[0];
        float maxZ = transf.z, minZ = transf.z;
        float maxX = transf.x, minX = transf.x;
        float maxY = transf.y, minY = transf.y;
        for (uint32_t i = 1; i < 8; ++i)
        {
            transf = lightViewMatrix * cameraFrustrum[i];

            if (transf.z > maxZ) maxZ = transf.z;
            if (transf.z < minZ) minZ = transf.z;
            if (transf.x > maxX) maxX = transf.x;
            if (transf.x < minX) minX = transf.x;
            if (transf.y > maxY) maxY = transf.y;
            if (transf.y < minY) minY = transf.y;
        }

        lightViewMatrix[3][0] = -(minX + maxX) * 0.5f;
        lightViewMatrix[3][1] = -(minY + maxY) * 0.5f;
        lightViewMatrix[3][2] = -(minZ + maxZ) * 0.5f;

        Vec3 halfExtents((maxX - minX) * 0.5, (maxY - minY) * 0.5, (maxZ - minZ) * 0.5);

        return OrthographicMatrix(-halfExtents.x, halfExtents.x, halfExtents.y, -halfExtents.y, halfExtents.z, -halfExtents.z) * lightViewMatrix;
    }


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, const EngineSettings::ShadowResolution shadowmapRes,
        const EngineSettings::ShadowReadbackLatency readbackLatency, const uint32_t windowWidth, const uint32_t windowHeight)
        :
        mContext(context),

        mPCF2x2Shader(nullptr),
        mPCF3x3Shader(nullptr),
        mPCF5x5Shader(nullptr),
        mPCF7x7Shader(nullptr),

        mRSDepthClamp(nullptr),
        mFullscreenPass(fullscreenPass),
        mVertexTransformPass(transformPass),
        mShadowmap(device, context, shadowmapRes, NUM_SHADOWMAP_CASCADES, false),
        mDirLightCBuffer(device, context, mDirLightCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF2X2PixelShader, sizeof(gDirectionalLightPCF2X2PixelShader), nullptr, &mPCF2x2Shader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF3X3PixelShader, sizeof(gDirectionalLightPCF3X3PixelShader), nullptr, &mPCF3x3Shader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF5X5PixelShader, sizeof(gDirectionalLightPCF5X5PixelShader), nullptr, &mPCF5x5Shader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPCF7X7PixelShader, sizeof(gDirectionalLightPCF7X7PixelShader), nullptr, &mPCF7x7Shader));

        // depth clamp to avoid meshes between frustrum split issues
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthBiasClamp = 1.0f;
        rasterizerDesc.DepthClipEnable = false;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mRSDepthClamp));
    }

    DX11DirectionalLightPass::~DX11DirectionalLightPass()
    {
    }


    void DX11DirectionalLightPass::Render(const RenderableDirLight& directionalLight, const EngineSettings::ShadowFiltering shadowFiltering, const float degreesFOV, const float aspectRatio, const Mat4& cameraViewMatrix, const Mat4& invCameraProjMatrix, const Vec2& windowSize, const Mat4& cameraProjMatrix)
    {
        // preserve current state
        D3D11_VIEWPORT prevViewport;
        ID3D11RasterizerStatePtr prevRS = nullptr;
        ID3D11RenderTargetViewPtr prevRTV = nullptr;
        ID3D11DepthStencilViewPtr prevDSV = nullptr;
        uint32_t numViewports = 1;
        mContext->RSGetViewports(&numViewports, &prevViewport);
        mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);
        mContext->RSGetState(&prevRS);


        //
        // Reduce Depth pass
        //

		std::array<float, NUM_SHADOWMAP_CASCADES> splitDistances;
		const float minZ = CalculateShadowmapCascades(cameraProjMatrix, splitDistances);


        //
        // Shadow pass
        //

        // unbind any set pixel shader
        mContext->PSSetShader(nullptr, nullptr, 0);

        // depth clamp to avoid issues with meshes between splits
        mContext->RSSetState(mRSDepthClamp);

        std::array<Mat4, NUM_SHADOWMAP_CASCADES> lightVPMatrices;

        mVertexTransformPass.BindForTransformPass(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mShadowmap.BindForDrawing();
        
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; ++cascadeIndex)
        {
            mShadowmap.BindDepthView(cascadeIndex);
            
            // get cascade frustrum
            const Mat4 perspectiveMatrix = PerspectiveMatrixFov(degreesFOV, aspectRatio, cascadeIndex == 0 ? minZ : splitDistances[cascadeIndex - 1], splitDistances[cascadeIndex]);
            CameraFrustrum cameraFrustrum = CalculateCameraFrustrum(perspectiveMatrix * cameraViewMatrix);

            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(cameraFrustrum, directionalLight.mLightDirection);
			mVertexTransformPass.RenderMeshes(directionalLight.mMeshes, lightVPMatrices[cascadeIndex]);

            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex] * glm::inverse(cameraViewMatrix);
        }

        // negate the split distances for comparison against view space position in the shader
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; ++cascadeIndex)
            splitDistances[cascadeIndex] = -splitDistances[cascadeIndex];


        //
        // Shading pass
        //
        
        // restore rendering to backbuffer, rasterize state and viewport
        mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
        mContext->RSSetViewports(numViewports, &prevViewport);
        mContext->RSSetState(prevRS);
        
        // bind shadowmap SRV for reading
        mShadowmap.BindForReading();

        const Vec4 camLightDir = glm::normalize(cameraViewMatrix * Vec4(-directionalLight.mLightDirection, 0));

        // set dir light cbuffer data
		mDirLightCBuffer.SetData(DirectionalLightCBuffer(lightVPMatrices, invCameraProjMatrix, splitDistances, directionalLight.mLightColor, camLightDir, windowSize, static_cast<float>(mShadowmap.GetTextureSize())));
        
        // bind appropiate shading pixel shader for shadow filtering argument
        BindShadingPixelShader(shadowFiltering);

        // run fullscreen pass + dir light shading pass
        mFullscreenPass.Render();
    }


	float DX11DirectionalLightPass::CalculateShadowmapCascades(const Mat4& cameraProjMatrix, std::array<float, NUM_SHADOWMAP_CASCADES>& splitDistances)
	{
		float minDepth, maxDepth;
		ReduceDepth(cameraProjMatrix, minDepth, maxDepth);

		const float nearClip = Z_NEAR;
		const float farClip = Z_FAR;
		const float clipRange = farClip - nearClip;

		const float minZ = nearClip + minDepth * clipRange;
		const float maxZ = nearClip + maxDepth * clipRange;

        const float range = maxZ - minZ;
        const float ratio = maxZ / minZ;

		for (uint32_t cascadeIndex = 0; cascadeIndex < DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES; ++cascadeIndex)
		{
			const float p = (cascadeIndex + 1) / static_cast<float>(DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES);
			const float log = minZ * std::pow(ratio, p);
			const float uniform = minZ + range * p;
			const float d = log - uniform + uniform;
			splitDistances[cascadeIndex] = (d - nearClip) / clipRange;

            splitDistances[cascadeIndex] *= farClip;
		}

		// minZ is used for first splits near distance
		return minZ;
	}

    void DX11DirectionalLightPass::BindShadingPixelShader(const EngineSettings::ShadowFiltering shadowFiltering)
    {
        switch (shadowFiltering)
        {
            default:
            case EngineSettings::ShadowFiltering::PCF_2X2:
            {
                mContext->PSSetShader(mPCF2x2Shader, nullptr, 0);
                break;
            }
            case EngineSettings::ShadowFiltering::PCF_3X3:
            {
                mContext->PSSetShader(mPCF3x3Shader, nullptr, 0);
                break;
            }
            case EngineSettings::ShadowFiltering::PCF_5X5:
            {
                mContext->PSSetShader(mPCF5x5Shader, nullptr, 0);
                break;
            }
            case EngineSettings::ShadowFiltering::PCF_7X7:
            {
                mContext->PSSetShader(mPCF7x7Shader, nullptr, 0);
                break;
            }
        }
    }
}