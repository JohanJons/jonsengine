#include "include/Renderer/DirectX11/DX11AmbientPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AmbientPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

namespace JonsEngine
{
    DX11AmbientPass::DX11AmbientPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const uint16_t screenWidth, const uint16_t screenHeight) :
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

        DXCALL(device->CreateTexture2D(&ssaoTextureDesc, NULL, &mSSAOTexture));
        DXCALL(device->CreateShaderResourceView(mSSAOTexture, NULL, &mSSAOSRV));
        DXCALL(device->CreateRenderTargetView(mSSAOTexture, NULL, &mSSAORTV));

        DXCALL(device->CreatePixelShader(gAmbientPixelShader, sizeof(gAmbientPixelShader), NULL, &mAmbientPixelShader));
        DXCALL(device->CreatePixelShader(gSSAOPixelShader, sizeof(gSSAOPixelShader), NULL, &mSSAOPixelShader));
    }

    DX11AmbientPass::~DX11AmbientPass()
    {
    }


    void DX11AmbientPass::Render(const Mat4& invCameraProjMatrix, const Vec4& ambientLight, const Vec2& screenSize, const bool useSSAO)
    {
        if (useSSAO)
        {
            ID3D11RenderTargetViewPtr prevRTV = nullptr;
            ID3D11DepthStencilViewPtr prevDSV = nullptr;
            mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);

            // pass 1: render AO to texture
            mContext->OMSetRenderTargets(1, &mSSAORTV.p, NULL);
            mContext->PSSetShader(mSSAOPixelShader, NULL, NULL);
            mSSAOCBuffer.SetData(SSAOCBuffer(invCameraProjMatrix, screenSize));
            mFullscreenPass.Render();

            // pass 2: horizontal + vertical blur
            // can keep SSAO texture as rendertarget aswell because box blur does two passes - one intermediate
            mContext->OMSetRenderTargets(1, &mSSAORTV.p, NULL);
            mBoxBlurPass.Render(mSSAOSRV, screenSize);

            // restore rendering to backbuffer
            mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
            // set SSAO texture as SRV
            mContext->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &mSSAOSRV.p);
        }

        // pass 3: render ambient light
        mContext->PSSetShader(mAmbientPixelShader, NULL, NULL);
        mAmbientCBuffer.SetData(AmbientCBuffer(ambientLight, useSSAO));

        mFullscreenPass.Render();
    }
}
