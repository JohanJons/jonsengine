#include "include/Renderer/DirectX11/DX11BoxBlurPass.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/BoxBlurPixel.h"

namespace JonsEngine
{
    DX11BoxBlurPass::DX11BoxBlurPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, const DXGI_FORMAT textureFormat, const uint16_t screenWidth, const uint16_t screenHeight) :
        mContext(context), mPixelShader(nullptr), mBoxBlurTexture(nullptr), mBoxBlurSRV(nullptr), mBoxBlurRTV(nullptr), mFullscreenPass(fullscreenPass), mBoxBlurCBuffer(device, context, mBoxBlurCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        D3D11_TEXTURE2D_DESC blurTextureDesc;
        ZeroMemory(&blurTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        blurTextureDesc.Width = screenWidth;
        blurTextureDesc.Height = screenHeight;
        blurTextureDesc.Format = textureFormat;
        blurTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        blurTextureDesc.MipLevels = 1;
        blurTextureDesc.ArraySize = 1;
        blurTextureDesc.SampleDesc.Count = 1;

        DXCALL(device->CreateTexture2D(&blurTextureDesc, NULL, &mBoxBlurTexture));
        DXCALL(device->CreateShaderResourceView(mBoxBlurTexture, NULL, &mBoxBlurSRV));
        DXCALL(device->CreateRenderTargetView(mBoxBlurTexture, NULL, &mBoxBlurRTV));

        DXCALL(device->CreatePixelShader(gBoxBlurPixelShader, sizeof(gBoxBlurPixelShader), NULL, &mPixelShader));
    }

    DX11BoxBlurPass::~DX11BoxBlurPass()
    {
    }


    void DX11BoxBlurPass::Render(ID3D11ShaderResourceViewPtr textureToBlur, const Vec2& screenSize)
    {
        ID3D11RenderTargetViewPtr prevRTV = nullptr;
        ID3D11DepthStencilViewPtr prevDSV = nullptr;
        mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);

        mContext->PSSetShader(mPixelShader, NULL, NULL);

        // horizontal pass
        mContext->OMSetRenderTargets(1, &mBoxBlurRTV.p, NULL);
        mContext->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &textureToBlur.p);
        mBoxBlurCBuffer.SetData(BoxBlurCBuffer(Vec2(1.0f / screenSize.x, 0.0f)));

        mFullscreenPass.Render();

        // vertical pass
        // first set extra slot to null to avoid dx warnings
        mContext->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &gNullSrv.p);
        mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
        mContext->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &mBoxBlurSRV.p);
        mBoxBlurCBuffer.SetData(BoxBlurCBuffer(Vec2(0.0f, 1.0f / screenSize.y)));

        mFullscreenPass.Render();
    }
}