#include "include/Renderer/DirectX11/DX11CPUDynamicBuffer.h"

namespace JonsEngine
{
    DX11CPUDynamicBuffer::DX11CPUDynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) :
        mDevice(device),
        mContext(context),
        mBuffer(nullptr),
        mBufferSRV(nullptr),
        mBufferSizeInBytes(0)
    {
    }


    void DX11CPUDynamicBuffer::Bind(const Shaderslot shaderSlot, const uint32_t bindSlot)
    {
        switch (shaderSlot)
        {
            case Shaderslot::Vertex:  mContext->VSSetShaderResources(bindSlot, 1, &mBufferSRV.p); break;
            case Shaderslot::Pixel:  mContext->PSSetShaderResources(bindSlot, 1, &mBufferSRV.p); break;
            default: break;
        }
    }

	void DX11CPUDynamicBuffer::SetDataAndTranspose(const std::vector<Mat4>& transforms)
	{
		for (const Mat4& transform : transforms)
			mTransposedTransforms.emplace_back(glm::transpose(transform));

		SetData(mTransposedTransforms);

		mTransposedTransforms.clear();
	}
}