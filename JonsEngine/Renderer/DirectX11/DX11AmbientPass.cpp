#include "Renderer/DirectX11/DX11AmbientPass.h"

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "Renderer/DirectX11/Shaders/Compiled/AmbientPixel.h"
#include "Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

namespace JonsEngine
{
    DX11AmbientPass::DX11AmbientPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const uint32_t screenWidth, const uint32_t screenHeight) :
        mContext(context),
        mSSAOPixelShader(nullptr),
        mAmbientPixelShader(nullptr),
        mSSAOTexture(nullptr),
        mSSAOSRV(nullptr),
        mSSAORTV(nullptr),

        mFullscreenPass(fullscreenPass),
        mBoxBlurPass(device, context, mFullscreenPass, DXGI_FORMAT_R8_UNORM, screenWidth, screenHeight),
        mAmbientCBuffer(device, context, mAmbientCBuffer.CONSTANT_BUFFER_SLOT_PIXEL),
        mSSAOCBuffer(device, context, mSSAOCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        D3D11_TEXTURE2D_DESC ssaoTextureDesc;
        ZeroMemory(&ssaoTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        ssaoTextureDesc.Width = screenWidth;
        ssaoTextureDesc.Height = screenHeight;
        // TODO: R8 enough? or R16? ...........
        ssaoTextureDesc.Format = DXGI_FORMAT_R8_UNORM;
        ssaoTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        ssaoTextureDesc.MipLevels = 1;
        ssaoTextureDesc.ArraySize = 1;
        ssaoTextureDesc.SampleDesc.Count = 1;

        DXCALL(device->CreateTexture2D(&ssaoTextureDesc, nullptr, &mSSAOTexture));
        DXCALL(device->CreateShaderResourceView(mSSAOTexture, nullptr, &mSSAOSRV));
        DXCALL(device->CreateRenderTargetView(mSSAOTexture, nullptr, &mSSAORTV));

        DXCALL(device->CreatePixelShader(gAmbientPixelShader, sizeof(gAmbientPixelShader), nullptr, &mAmbientPixelShader));
        DXCALL(device->CreatePixelShader(gSSAOPixelShader, sizeof(gSSAOPixelShader), nullptr, &mSSAOPixelShader));
    }

    DX11AmbientPass::~DX11AmbientPass()
    {
    }


    void DX11AmbientPass::Render(const Vec4& ambientLight, const Vec2& windowSize, const bool useSSAO)
    {
        if (useSSAO)
        {
            ID3D11RenderTargetViewPtr prevRTV = nullptr;
            ID3D11DepthStencilViewPtr prevDSV = nullptr;
            mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);

            // pass 1: render AO to texture
            mContext->OMSetRenderTargets(1, &mSSAORTV.p, nullptr);
            mContext->PSSetShader(mSSAOPixelShader, nullptr, 0);
            mSSAOCBuffer.SetData(SSAOCBuffer(windowSize));
			mSSAOCBuffer.Bind();
            mFullscreenPass.Render();

            // pass 2: horizontal + vertical blur
            // can keep SSAO texture as rendertarget aswell because box blur does two passes - one intermediate
            mContext->OMSetRenderTargets(1, &mSSAORTV.p, nullptr);
            mBoxBlurPass.Render(mSSAOSRV, windowSize);

            // restore rendering to backbuffer
            mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
            // set SSAO texture as SRV
            mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mSSAOSRV.p);
        }

        // pass 3: render ambient light
        mContext->PSSetShader(mAmbientPixelShader, nullptr, 0);
        mAmbientCBuffer.SetData(AmbientCBuffer(ambientLight, useSSAO));
		mAmbientCBuffer.Bind();

        mFullscreenPass.Render();
    }
}
