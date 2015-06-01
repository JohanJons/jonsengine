#include "include/Renderer/DirectX11/DX11Pipeline.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"


namespace JonsEngine
{
    const DX11Color gClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Pipeline::DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, const EngineSettings::ShadowResolution shadowmapResolution,
        const EngineSettings::ShadowReadbackLatency shadowmapReadbackLatency, IDMap<DX11Mesh>& meshMap, IDMap<DX11Texture>& textureMap) 
        :
        mLogger(logger),
        mWindowSize(backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mMeshMap(meshMap),
        mTextureMap(textureMap),

        mSwapchain(swapchain),
        mContext(context),
        mDSV(nullptr),
        mDSVReadOnly(nullptr),
        mDepthSRV(nullptr),
        mDepthStencilState(nullptr),
        mAdditiveBlending(nullptr),
        
        mVertexTransformPass(device, context, meshMap),
        mAABBPass(device, context, mVertexTransformPass),
        mFullscreenTrianglePass(device, context),

        mBackbuffer(device, mContext, mSwapchain, mFullscreenTrianglePass),
        mLightAccbuffer(device, mContext, backbufferTextureDesc),
        mGBuffer(device, mContext, backbufferTextureDesc),

        mAmbientPass(device, context, mFullscreenTrianglePass, backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mDirectionalLightPass(device, mContext, mFullscreenTrianglePass, mVertexTransformPass, shadowmapResolution, shadowmapReadbackLatency, backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mPointLightPass(device, mContext, mVertexTransformPass, shadowmapResolution),
        mPostProcessor(device, context, mFullscreenTrianglePass, backbufferTextureDesc)
    {
        // create depth buffer/view/srv
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthStencilBufferDesc.Width = backbufferTextureDesc.Width;
        depthStencilBufferDesc.Height = backbufferTextureDesc.Height;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        DXCALL(device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &mDepthStencilBuffer));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DXCALL(device->CreateShaderResourceView(mDepthStencilBuffer, &srvDesc, &mDepthSRV));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDSV));

        dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDSVReadOnly));

        // depth stencil config used in shading stage
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = false;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));

        // create blend state for shading pass
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        DXCALL(device->CreateBlendState(&blendDesc, &mAdditiveBlending));
    }

    DX11Pipeline::~DX11Pipeline()
    {
    }


    void DX11Pipeline::BeginFrame()
    {
        mBackbuffer.ClearBackbuffer(gClearColor);
    }

    void DX11Pipeline::EndFrame()
    {
        DXCALL(mSwapchain->Present(0, 0));
    }


    void DX11Pipeline::GeometryStage(const RenderQueue& renderQueue)
    {
        mGBuffer.BindForGeometryStage(mDSV);

		for (const RenderableModel& model : renderQueue.mCamera.mModels)
        {
            assert(model.mMesh.mMeshID != INVALID_MESH_ID);

            const bool hasDiffuseTexture = model.mMaterial.mDiffuseTextureID != INVALID_TEXTURE_ID;
            const bool hasNormalTexture = model.mMaterial.mNormalTextureID != INVALID_TEXTURE_ID;

            if (hasDiffuseTexture)
                mTextureMap.GetItem(model.mMaterial.mDiffuseTextureID).Bind();

            if (hasNormalTexture)
                mTextureMap.GetItem(model.mMaterial.mNormalTextureID).Bind();

            const Mat4& localTransform = renderQueue.mLocalTransformStorage.GetItem(model.mMesh.mLocalTransformID);
            const Mat4& worldTransform = renderQueue.mWorldTransformStorage.GetItem(model.mMesh.mWorldTransformID);
            const Mat4 localWorldMatrix = worldTransform * localTransform;

            mGBuffer.SetConstantData(renderQueue.mCamera.mCameraViewProjectionMatrix * localWorldMatrix, renderQueue.mCamera.mCameraViewMatrix * localWorldMatrix, model.mTextureTilingFactor, hasDiffuseTexture, hasNormalTexture);
            mMeshMap.GetItem(model.mMesh.mMeshID).Draw();
        }
    }

    void DX11Pipeline::LightingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugExtra, const EngineSettings::ShadowFiltering shadowFiltering, const bool SSAOEnabled)
    {
        mLightAccbuffer.ClearAccumulationBuffer();
        mLightAccbuffer.BindForDrawing(mDSVReadOnly);
        mGBuffer.BindGeometryTextures();

		// TODO: move elsewhere?
        mContext->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);
		mContext->CSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);

        // disable further depth writing
        mContext->OMSetDepthStencilState(mDepthStencilState, 0);

        const Mat4 invCameraProjMatrix = glm::inverse(renderQueue.mCamera.mCameraProjectionMatrix);

        // ambient light
        mAmbientPass.Render(invCameraProjMatrix, renderQueue.mAmbientLight, mWindowSize, SSAOEnabled);

        // additive blending for adding lighting
        mContext->OMSetBlendState(mAdditiveBlending, nullptr, 0xffffffff);

        // do all directional lights
        for (const RenderableDirLight& directionalLight : renderQueue.mDirectionalLights)
            mDirectionalLightPass.Render(directionalLight, renderQueue.mLocalTransformStorage, renderQueue.mWorldTransformStorage, shadowFiltering, renderQueue.mCamera.mFOV, mWindowSize.x / mWindowSize.y, renderQueue.mCamera.mCameraViewMatrix, invCameraProjMatrix, mWindowSize, renderQueue.mCamera.mCameraProjectionMatrix);

        // do all point lights
        mPointLightPass.BindForShading();
        for (const RenderablePointLight& pointLight : renderQueue.mPointLights)
        {
            mContext->ClearDepthStencilView(mDSV, D3D11_CLEAR_STENCIL, 1.0f, 0);
            mPointLightPass.Render(pointLight, renderQueue.mLocalTransformStorage, renderQueue.mWorldTransformStorage, renderQueue.mCamera.mCameraViewMatrix, renderQueue.mCamera.mCameraViewProjectionMatrix, invCameraProjMatrix, mWindowSize, Z_FAR, Z_NEAR);
        }

        // turn off blending
        mContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    }

    void DX11Pipeline::PostProcessingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags, const EngineSettings::AntiAliasing AA)
    {
        // flip from lightAccumulatorBuffer --> backbuffer
        mBackbuffer.FillBackbuffer(mLightAccbuffer.GetLightAccumulationBuffer(), true);

        //  post-processing done in sRGB space
        mBackbuffer.BindForDrawing();

        // FXAA done in sRGB space
        if (AA == EngineSettings::AntiAliasing::FXAA)
            mPostProcessor.FXAAPass(mBackbuffer, mWindowSize);

        if (debugFlags.test(DebugOptions::RENDER_FLAG_DRAW_AABB))
            mAABBPass.Render(renderQueue);
    }
}