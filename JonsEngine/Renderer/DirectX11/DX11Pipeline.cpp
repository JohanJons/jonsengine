#include "Renderer/DirectX11/DX11Pipeline.h"

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/RenderSettings.h"
#include "Compiled/SimpleTexturePixel.h"
#include "RenderQueue/RenderQueue.h"

namespace JonsEngine
{
    DX11Pipeline::DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, DX11Backbuffer& backbuffer, const RenderSettings& settings, IDMap<DX11Mesh>& meshMap, IDMap<DX11Texture>& textureMap)
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
		mSimpleTextureShader(nullptr),

        mBoneTransformsBuffer(device, context),
        mVertexTransformPass(device, context, meshMap),
        mAABBPass(device, context, mVertexTransformPass),
        mFullscreenPass(device, context),

		mBackbuffer(backbuffer),
        mLightAccbuffer(device, mContext, backbufferTextureDesc),
        mGBuffer(device, mContext, backbufferTextureDesc),

        mAmbientPass(device, context, mFullscreenPass, backbufferTextureDesc.Width, backbufferTextureDesc.Height),
        mDirectionalLightPass( device, mContext, mFullscreenPass, mVertexTransformPass, settings.mShadowResolution, settings.mShadowReadbackLatency ),
        mPointLightPass(device, mContext, mVertexTransformPass, settings.mShadowResolution),
		mToneMapper(device, context, mFullscreenPass),
        mPostProcessor(device, context, mFullscreenPass, backbufferTextureDesc),
        mSkyboxPass(device, context),

		mTerrainPass(device, context, mVertexTransformPass, mTextureMap, settings.mTerrainPatchSize),
		mPerFrameCB(device, context, mPerFrameCB.CONSTANT_BUFFER_SLOT_PER_FRAME)
    {
		auto depthStencilBuffer = backbuffer.GetDepthbuffer();

        // create depth buffer view/srv
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DXCALL(device->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &mDepthSRV));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DXCALL(device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &mDSV));

        dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
        DXCALL(device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &mDSVReadOnly));

        // depth stencil config used in shading stage
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
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

		// pixelshader that will output lightAccumBuffer to backbuffer
		DXCALL(device->CreatePixelShader(gSimpleTexturePixel, sizeof(gSimpleTexturePixel), nullptr, &mSimpleTextureShader))
    }


    void DX11Pipeline::BeginFrame( const RenderQueue& renderQueue, const RenderSettings& renderSettings )
    {
        mBackbuffer.ClearBackbuffer(gClearColor);

		SetPerFrameCBuffer( renderQueue, renderSettings );

		// Send all bone transforms to GPU
		const bool hasBoneData = !renderQueue.mRenderData.mBones.empty();
		if (hasBoneData)
		{
			mBoneTransformsBuffer.SetData(renderQueue.mRenderData.mBones);
			mBoneTransformsBuffer.Bind(DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_BONE_TRANSFORMS);
		}
    }

    void DX11Pipeline::EndFrame( const RenderSettings& renderSettings )
    {
        if ( renderSettings.mVSync )
        {
            DXCALL(mSwapchain->Present(1, 0));
        }
        else
        {
            DXCALL(mSwapchain->Present(0, 0));
        }
    }


    void DX11Pipeline::GeometryStage( const RenderQueue& renderQueue, const RenderSettings& renderSettings )
    {
        mGBuffer.BindForGeometryStage(mDSV);

		mTerrainPass.Render( renderQueue.mTerrains, renderSettings.mTerrainPatchSize );

		mGBuffer.BindForRendering();

        // static meshes
		const auto staticBeginIndex = renderQueue.mCamera.mStaticMeshesBegin;
		const auto staticEndIndex = renderQueue.mCamera.mStaticMeshesEnd;
		const auto& staticMeshesContainer = renderQueue.mRenderData.mStaticMeshes;
		mGBuffer.BindForStaticPass();
		RenderMeshes(renderQueue, staticMeshesContainer, staticBeginIndex, staticEndIndex);
        
        // animated meshes
        const auto animatedBeginIndex = renderQueue.mCamera.mAnimatedMeshesBegin;
		const auto animatedEndIndex = renderQueue.mCamera.mAnimatedMeshesEnd;
		const auto& animatedMeshesContainer = renderQueue.mRenderData.mAnimatedMeshes;
		mGBuffer.BindForAnimatedPass();
		RenderMeshes(renderQueue, animatedMeshesContainer, animatedBeginIndex, animatedEndIndex);
    }

    void DX11Pipeline::LightingStage(const RenderQueue& renderQueue, const RenderSettings& renderSettings)
    {
        mLightAccbuffer.Clear();
        mLightAccbuffer.BindAsRenderTarget(mDSVReadOnly);
        mGBuffer.BindGeometryTextures();

        // expose depth buffer as SRV
		// TODO: move elsewhere?
        mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);
		mContext->CSSetShaderResources(SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);

        // disable further depth writing
        mContext->OMSetDepthStencilState(mDepthStencilState, 0);

        mAmbientPass.Render(renderQueue.mAmbientLight, mWindowSize, renderSettings.mSSAOEnabled);

        // additive blending for adding lighting
        mContext->OMSetBlendState(mAdditiveBlending, nullptr, 0xffffffff);

        for (const RenderableDirectionalLight& directionalLight : renderQueue.mDirectionalLights)
            mDirectionalLightPass.Render(directionalLight, renderQueue.mRenderData, renderSettings.mShadowFiltering, renderQueue.mCamera.mFOV, renderQueue.mWindowAspectRatio, renderQueue.mCamera.mCameraViewMatrix);

        for (const RenderablePointLight& pointLight : renderQueue.mPointLights)
        {
            mContext->ClearDepthStencilView(mDSV, D3D11_CLEAR_STENCIL, 1.0f, 0);
            mPointLightPass.Render(pointLight, renderQueue.mRenderData, renderQueue.mCamera.mCameraViewMatrix, renderQueue.mCamera.mCameraViewProjectionMatrix);
        }

        // turn off blending
        mContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		if (renderQueue.mSkyboxTextureID != INVALID_DX11_TEXTURE_ID)
			mSkyboxPass.Render(mTextureMap.GetItem(renderQueue.mSkyboxTextureID));
    }

    void DX11Pipeline::PostProcessingStage(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const RenderSettings& renderSettings)
    {
		PerformTonemapping(elapstedFrameTime, renderSettings.mAutoExposureRate, renderSettings.mToneMapping);

        // rest of post-processing done in sRGB space directly to the backbuffer
        mBackbuffer.BindForPostProcessing();

        // FXAA done in sRGB space
        if (renderSettings.mAntiAliasing == RenderSettings::AntiAliasing::Fxaa)
            mPostProcessor.FXAAPass(mBackbuffer, mWindowSize);
    }

	void DX11Pipeline::DebugStage( const RenderQueue& renderQueue, const RenderSettings& renderSettings, const DebugOptions::RenderingFlags debugFlags )
	{
		bool renderAABBs = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_MODEL_AABB ) || debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_AABB );
		if ( renderAABBs )
			mAABBPass.Render( renderQueue.mColorsToAABBsList, renderQueue.mCamera.mCameraViewProjectionMatrix );

		bool drawCoplanarity = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_COPLANARITY );
		bool drawWireframe = drawCoplanarity || debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_WIREFRAME );
		if ( drawWireframe )
			mTerrainPass.RenderDebug( renderQueue.mTerrains, renderSettings.mTerrainPatchSize, drawCoplanarity );
	}

	void DX11Pipeline::RenderMeshes(const RenderQueue& renderQueue, const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index begin, const RenderableMesh::Index end)
	{
		for (auto meshIndex = begin; meshIndex < end; ++meshIndex)
		{
			const RenderableMesh& mesh = meshContainer.at(meshIndex);
			assert(mesh.mMeshID != INVALID_DX11_MESH_ID);

			bool hasDiffuseTexture = false, hasNormalTexture = false;
			if (mesh.mMaterial != RenderableMaterial::INVALID_INDEX)
			{
				const RenderableMaterial& material = renderQueue.mRenderData.mMaterials.at(mesh.mMaterial);
				hasDiffuseTexture = material.mDiffuseTextureID != INVALID_DX11_TEXTURE_ID;
				hasNormalTexture = material.mNormalTextureID != INVALID_DX11_TEXTURE_ID;

				if (hasDiffuseTexture)
					mTextureMap.GetItem(material.mDiffuseTextureID).BindAsShaderResource(SHADER_TEXTURE_SLOT_DIFFUSE);

				if (hasNormalTexture)
					mTextureMap.GetItem(material.mNormalTextureID).BindAsShaderResource(SHADER_TEXTURE_SLOT_NORMAL);
			}

			uint32_t boneOffset = 0;
			const bool hasBones = mesh.mSkeleton.mBoneRange.first != INVALID_BONE_INDEX;
			if (hasBones)
				boneOffset = mesh.mSkeleton.mBoneRange.first;

			const Mat4 wvpMatrix = renderQueue.mCamera.mCameraViewProjectionMatrix * mesh.mLocalWorldTransform;
			const Mat4 worldViewMatrix = renderQueue.mCamera.mCameraViewMatrix * mesh.mLocalWorldTransform;

			mGBuffer.SetConstantData(wvpMatrix, worldViewMatrix, mesh.mMaterialTilingFactor, hasDiffuseTexture, hasNormalTexture, boneOffset);
			mMeshMap.GetItem(mesh.mMeshID).Draw();
		}
	}

	void DX11Pipeline::PerformTonemapping(const Milliseconds elapstedFrameTime, const RenderSettings::AutoExposureRate exposureRate, const RenderSettings::ToneMappingAlghorithm alghorithm)
	{
		if (alghorithm != RenderSettings::ToneMappingAlghorithm::None)
		{
			// render luminance to texture
			mToneMapper.BindAsRenderTarget();
			mLightAccbuffer.BindAsShaderResource();
			mToneMapper.RenderLuminance(elapstedFrameTime, exposureRate);

			// flip from lightAccumulatorBuffer --> backbuffer using tonemapping
			mBackbuffer.BindForTonemapping();
			mToneMapper.ApplyToneMapping(alghorithm);
		}
		else
		{
			// simply copy lightAccumulatorBuffer --> backbuffer with sRGB conversion
			mBackbuffer.BindForTonemapping();
			mLightAccbuffer.BindAsShaderResource();
			mContext->PSSetShader(mSimpleTextureShader, nullptr, 0);
			mFullscreenPass.Render();
		}
	}

	void DX11Pipeline::SetPerFrameCBuffer( const RenderQueue& renderQueue, const RenderSettings& renderSettings )
	{
		auto& camera = renderQueue.mCamera;

		mPerFrameCB.SetData({ 
			camera.mCameraViewProjectionMatrix,
			camera.mCameraViewMatrix,
			glm::inverse(camera.mCameraViewMatrix),
			glm::inverse(camera.mCameraProjectionMatrix),
			camera.mCameraPosition,
			Vec2( Z_NEAR, Z_FAR ),
			renderQueue.mWindowDimensions,
			RenderSettingsToVal( renderSettings.mTerrainPatchSize ),
			RenderSettingsToVal( renderSettings.mTerrainPrimitiveLength ),
			RenderSettingsToVal( renderSettings.mTerrainTessellationMax ),
			RenderSettingsToVal( renderSettings.mTerrainCoplanarityScale )
		});
		mPerFrameCB.Bind();
	}
}