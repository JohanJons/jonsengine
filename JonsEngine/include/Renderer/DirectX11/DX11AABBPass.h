#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/RenderQueue/RenderQueueTypes.h"
#include "include/Core/Math/AABB.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class DX11VertexTransformPass;

	// TODO: refactor into something else?

    class DX11AABBPass
    {
    public:
        DX11AABBPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass);
        ~DX11AABBPass();

        void Render( const RenderableAABBsContainer& renderData, const Mat4& viewProj );


    private:
		struct AABBCBuffer
		{
			Vec3 mColor;
			float __padding;
		};

        ID3D11DeviceContextPtr mContext;
        ID3D11PixelShaderPtr mPixelShader;

		AABBCBuffer mAABBCBuffer;
        DX11VertexTransformPass& mVertexTransformPass;
		DX11ConstantBuffer<AABBCBuffer> mCBuffer;
    };
}
