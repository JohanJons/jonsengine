#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"

#include "include/Renderer/DirectX11/Shaders/Compiled/TransformVertex.h"

namespace JonsEngine
{
    DX11VertexTransformPass::DX11VertexTransformPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const IDMap<DX11Mesh>& meshMap) :
        mContext(context), mTransformCBuffer(device, context, mTransformCBuffer.CONSTANT_BUFFER_SLOT_VERTEX), mMeshMap(meshMap)
    {
        D3D11_INPUT_ELEMENT_DESC inputDescription;
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription.SemanticName = "POSITION";
        inputDescription.SemanticIndex = 0;
        inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription.InputSlot = 0;
        inputDescription.AlignedByteOffset = 0;
        inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription.InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(&inputDescription, 1, gTransformVertexShader, sizeof(gTransformVertexShader), &mInputLayout));

        DXCALL(device->CreateVertexShader(gTransformVertexShader, sizeof(gTransformVertexShader), NULL, &mVertexShader));
    }

    DX11VertexTransformPass::~DX11VertexTransformPass()
    {
    }


    void DX11VertexTransformPass::BindForTransformPass(const D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
    {
        mContext->IASetPrimitiveTopology(primitiveTopology);
        mContext->IASetInputLayout(mInputLayout);

        mContext->VSSetShader(mVertexShader, NULL, NULL);
    }

    void DX11VertexTransformPass::RenderMesh(DX11Mesh& mesh, const Mat4& wvpMatrix)
    {
        mTransformCBuffer.SetData(TransformCBuffer(wvpMatrix));
        mesh.Draw();
    }

    void DX11VertexTransformPass::RenderMeshes(const RenderableMeshes& meshes, const Mat4& viewProjectionMatrix)
    {
        for (const RenderableMesh& mesh : meshes)
        {
            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldMatrix));
            mMeshMap.GetItem(mesh.mMeshID).Draw();
        }
    }

    void DX11VertexTransformPass::RenderAABBs(const RenderableMeshes& meshes, const Mat4& viewProjectionMatrix)
    {
        for (const RenderableMesh& mesh : meshes)
        {
            mTransformCBuffer.SetData(TransformCBuffer(viewProjectionMatrix * mesh.mWorldMatrix));
            mMeshMap.GetItem(mesh.mMeshID).DrawAABB();
        }
    }
}