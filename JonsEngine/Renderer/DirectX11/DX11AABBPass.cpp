#include "Renderer/DirectX11/DX11AABBPass.h"

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11VertexTransformPass.h"
#include "Compiled/SimpleColorPixel.h"

namespace JonsEngine
{
    DX11AABBPass::DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass) :
		mContext( context ),
		mPixelShader( nullptr ),
		mVertexTransformPass( vertexTransformPass ),
		mCBuffer( device, context, mCBuffer.CONSTANT_BUFFER_SLOT_PIXEL )
    {
        DXCALL(device->CreatePixelShader(gSimpleColorPixel, sizeof(gSimpleColorPixel), nullptr, &mPixelShader));
    }
    
    DX11AABBPass::~DX11AABBPass()
    {
    }


    void DX11AABBPass::Render(const RenderableAABBsContainer& renderData, const Mat4& viewProj )
    {
        mContext->PSSetShader(mPixelShader, nullptr, 0);

		for ( const AABBsOfColor& AABBsByColor : renderData )
		{
			mAABBCBuffer.mColor = AABBsByColor.first;
			mCBuffer.SetData( mAABBCBuffer );
			mCBuffer.Bind();

			mVertexTransformPass.RenderAABBs( AABBsByColor.second, viewProj );
		}
    }
}