#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangle.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleTexcoord.h"

namespace JonsEngine
{
    DX11FullscreenTrianglePass::DX11FullscreenTrianglePass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) :
        mContext(context),
		mFSVertexShader(nullptr),
		mFSTexcoordVertexShader(nullptr)
    {
        DXCALL(device->CreateVertexShader(gFullscreenTriangleShader, sizeof(gFullscreenTriangleShader), nullptr, &mFSVertexShader));
		DXCALL(device->CreateVertexShader(gFullscreenTriangleTexcoordShader, sizeof(gFullscreenTriangleTexcoordShader), nullptr, &mFSTexcoordVertexShader));
    }

    DX11FullscreenTrianglePass::~DX11FullscreenTrianglePass()
    {
    }


    void DX11FullscreenTrianglePass::Render(const bool withTexcoords)
    {
        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mContext->IASetInputLayout(nullptr);

        mContext->VSSetShader(withTexcoords ? mFSTexcoordVertexShader : mFSVertexShader, nullptr, 0);

        mContext->Draw(3, 0);
    }
}