#include "include/Renderer/DirectX11/DX11PostProcessor.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FXAAPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

#include <random>

namespace JonsEngine
{
    Vec4 noiseTexture[16];
    ID3D11Texture2DPtr gTexture = nullptr;
    ID3D11ShaderResourceViewPtr mShaderResourceView = nullptr;

    DX11PostProcessor::DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mFullscreenPass(fullscreenPass), mFXAACBuffer(device, mFXAACBuffer.CONSTANT_BUFFER_SLOT_PIXEL), mSSAOCBuffer(device, mSSAOCBuffer.CONSTANT_BUFFER_SLOT_PIXEL), mTexture(nullptr),
        mSRV(nullptr), mRTV(nullptr), mFXAAPixelShader(nullptr), mSSAOPixelShader(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mTexture));
        DXCALL(device->CreateRenderTargetView(mTexture, NULL, &mRTV));
        DXCALL(device->CreateShaderResourceView(mTexture, NULL, &mSRV));
        DXCALL(device->CreatePixelShader(gFXAAPixelShader, sizeof(gFXAAPixelShader), NULL, &mFXAAPixelShader));
        DXCALL(device->CreatePixelShader(gSSAOPixelShader, sizeof(gSSAOPixelShader), NULL, &mSSAOPixelShader));



        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = 4;
        textureDesc.Height = 4;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &gTexture));
        DXCALL(device->CreateShaderResourceView(gTexture, NULL, &mShaderResourceView));



        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> dist(-1.0f, 1.0f);
        for (int i = 0; i < 16; i++)
        {
            noiseTexture[i] = Vec4(dist(e2), dist(e2), 0.0f, 0.0f);
            glm::normalize(noiseTexture[i]);
        }
    }

    DX11PostProcessor::~DX11PostProcessor()
    {
    }


    void DX11PostProcessor::FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer, const Vec2& screenSize)
    {
        backbuffer.CopyBackbufferTexture(context, mTexture);

        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE, 1, &mSRV.p);
        context->PSSetShader(mFXAAPixelShader, NULL, NULL);
        mFXAACBuffer.SetData(FXAACBuffer(screenSize), context);

        mFullscreenPass.Render(context);
    }

    void DX11PostProcessor::SSAOPass(ID3D11DeviceContextPtr context, const Mat4& camViewProjMatrix, const Mat4& projMatrix, const Mat4& viewMatrix, const Vec2& screenSize)
    {
        uint32_t sizeWidth = 4 * sizeof(float) * 4;
        context->UpdateSubresource(gTexture, 0, NULL, &noiseTexture[0], sizeWidth, 0);

        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE, 1, &mShaderResourceView.p);     // noise texture
        context->PSSetShader(mSSAOPixelShader, NULL, NULL);
        mSSAOCBuffer.SetData(SSAOCBuffer(camViewProjMatrix, projMatrix, viewMatrix, screenSize), context);

        mFullscreenPass.Render(context);
    }
}