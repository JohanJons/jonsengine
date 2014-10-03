#include "include/Renderer/DirectX11/DX11BoxBlurPass.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/BoxBlurPixel.h"

namespace JonsEngine
{
    DX11BoxBlurPass::DX11BoxBlurPass(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass) : mFullscreenPass(fullscreenPass), mBoxBlurCBuffer(device, mBoxBlurCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        DXCALL(device->CreatePixelShader(gBoxBlurPixelShader, sizeof(gBoxBlurPixelShader), NULL, &mPixelShader));
    }

    DX11BoxBlurPass::~DX11BoxBlurPass()
    {
    }


    void DX11BoxBlurPass::Render(ID3D11DeviceContextPtr context, ID3D11ShaderResourceViewPtr srv, const Vec2& texelScale)
    {
        context->PSSetShader(mPixelShader, NULL, NULL);
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE, 1, &srv.p);
        mBoxBlurCBuffer.SetData(BoxBlurCBuffer(texelScale), context);

        mFullscreenPass.Render(context);
    }
}