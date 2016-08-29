#include "include/Renderer/DirectX11/DX11PostProcessor.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FXAAPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

namespace JonsEngine
{
    DX11PostProcessor::DX11PostProcessor(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mContext(context),
        mFullscreenPass(fullscreenPass),
        mFXAACBuffer(device, context, mFXAACBuffer.CONSTANT_BUFFER_SLOT_PIXEL),
        mFXAABackbufferTexture(nullptr),
        mFXAARTV(nullptr),
        mFXAASRV(nullptr),
        mFXAAPixelShader(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, nullptr, &mFXAABackbufferTexture));
        DXCALL(device->CreateRenderTargetView(mFXAABackbufferTexture, nullptr, &mFXAARTV));
        DXCALL(device->CreateShaderResourceView(mFXAABackbufferTexture, nullptr, &mFXAASRV));
        DXCALL(device->CreatePixelShader(gFXAAPixelShader, sizeof(gFXAAPixelShader), nullptr, &mFXAAPixelShader));
    }

    DX11PostProcessor::~DX11PostProcessor()
    {
    }


    void DX11PostProcessor::FXAAPass(DX11Backbuffer& backbuffer, const Vec2& windowSize)
    {
        backbuffer.CopyBackbuffer(mFXAABackbufferTexture);

        mContext->PSSetShaderResources(SHADER_TEXTURE_SLOT_EXTRA, 1, &mFXAASRV.p);
        mContext->PSSetShader(mFXAAPixelShader, nullptr, 0);
        mFXAACBuffer.SetData(FXAACBuffer(windowSize));

        mFullscreenPass.Render();
    }
}