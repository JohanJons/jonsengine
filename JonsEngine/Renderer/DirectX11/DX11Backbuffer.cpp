#include "Renderer/DirectX11/DX11Backbuffer.h"

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11FullscreenTrianglePass.h"


namespace JonsEngine
{
    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDXGISwapChainPtr swapchain) :
        mContext(context),
        mBackbufferTexture(nullptr),
		mDepthStencilBuffer(nullptr),
        mRTV(nullptr),
        mRTV_SRGB(nullptr)
    {
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mBackbufferTexture));

		D3D11_TEXTURE2D_DESC backbufferTextureDesc;
		ZeroMemory(&backbufferTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		mBackbufferTexture->GetDesc(&backbufferTextureDesc);

		// create depth buffer
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

        // create backbuffers RTVs
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        DXCALL(device->CreateRenderTargetView(mBackbufferTexture, &rtvDesc, &mRTV));

        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        DXCALL(device->CreateRenderTargetView(mBackbufferTexture, &rtvDesc, &mRTV_SRGB));
    }

    DX11Backbuffer::~DX11Backbuffer()
    {
    }


	ID3D11Texture2DPtr DX11Backbuffer::GetDepthbuffer()
	{
		return mDepthStencilBuffer;
	}


    void DX11Backbuffer::CopyBackbuffer(ID3D11Texture2DPtr dest)
    {
        mContext->CopyResource(dest, mBackbufferTexture);
    }


	void DX11Backbuffer::BindForTonemapping()
	{
		mContext->OMSetRenderTargets(1, &mRTV_SRGB.p, nullptr);
	}

	void DX11Backbuffer::BindForPostProcessing()
	{
		mContext->OMSetRenderTargets(1, &mRTV.p, nullptr);
	}


    void DX11Backbuffer::ClearBackbuffer(const DX11Color& clearColor)
    {
        mContext->ClearRenderTargetView(mRTV, GetClearColor());
    }
}