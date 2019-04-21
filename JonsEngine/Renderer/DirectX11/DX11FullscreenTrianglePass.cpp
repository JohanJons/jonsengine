#include "Renderer/DirectX11/DX11FullscreenTrianglePass.h"

#include "Renderer/DirectX11/Shaders/Compiled/FullscreenTriangle.h"
#include "Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleTexcoord.h"

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


    void DX11FullscreenTrianglePass::Render()
    {
        mContext->VSSetShader(mFSVertexShader, nullptr, 0);

		InternalRender();
    }

	void DX11FullscreenTrianglePass::RenderWithTexcoords()
	{
		mContext->VSSetShader(mFSTexcoordVertexShader, nullptr, 0);

		InternalRender();
	}


	void DX11FullscreenTrianglePass::InternalRender()
	{
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		mContext->IASetInputLayout(nullptr);

		mContext->Draw(3, 0);
	}
}