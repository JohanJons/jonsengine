#include "include/Renderer/DirectX11/DX11AmbientPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/AmbientPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

namespace JonsEngine
{
    DX11AmbientPass::DX11AmbientPass(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass) :
        mFullscreenPass(fullscreenPass), mBoxBlurPass(device, mFullscreenPass, DXGI_FORMAT_R8_UNORM), mAmbientPixelShader(nullptr), mAmbientCBuffer(device, mAmbientCBuffer.CONSTANT_BUFFER_SLOT_PIXEL),
        mSSAOCBuffer(device, mSSAOCBuffer.CONSTANT_BUFFER_SLOT_PIXEL), mSSAOTexture(nullptr), mSSAOSRV(nullptr), mSSAORTV(nullptr)
    {
        D3D11_TEXTURE2D_DESC ssaoTextureDesc;
        ZeroMemory(&ssaoTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        // TODO: real size
        ssaoTextureDesc.Width = 1920;
        ssaoTextureDesc.Height = 1080;
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


    void DX11AmbientPass::Render(ID3D11DeviceContextPtr context, const Mat4& viewMatrix, const Vec4& ambientLight, const Vec2& screenSize, const bool useSSAO)
    {
        if (useSSAO)
        {
            ID3D11RenderTargetViewPtr prevRTV = nullptr;
            ID3D11DepthStencilViewPtr prevDSV = nullptr;
            context->OMGetRenderTargets(1, &prevRTV, &prevDSV);

            // pass 1: render AO to texture
            context->OMSetRenderTargets(1, &mSSAORTV.p, NULL);
            context->PSSetShader(mSSAOPixelShader, NULL, NULL);
            mSSAOCBuffer.SetData(SSAOCBuffer(viewMatrix), context);

            mFullscreenPass.Render(context);

            // pass 2: horizontal + vertical blur
            // can keep SSAO texture as rendertarget aswell because box blur does two passes - one intermediate
            context->OMSetRenderTargets(1, &mSSAORTV.p, NULL);
            mBoxBlurPass.Render(context, mSSAOSRV, screenSize);

            // restore rendering to backbuffer
            context->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
            // set SSAO texture as SRV
            context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &mSSAOSRV.p);
        }

        // pass 3: render ambient light
        context->PSSetShader(mAmbientPixelShader, NULL, NULL);
        mAmbientCBuffer.SetData(AmbientCBuffer(ambientLight, useSSAO), context);

        mFullscreenPass.Render(context);
    }
}
