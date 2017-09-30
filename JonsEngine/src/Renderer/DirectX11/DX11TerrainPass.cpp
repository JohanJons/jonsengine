#include "include/Renderer/DirectX11/DX11TerrainPass.h"

#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainHull.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainDomain.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainPixel.h"
#include "include/Core/Math/Math.h"

namespace JonsEngine
{
	const std::vector<float> gQuadVertices{
		-1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -1.0f
	};

	const std::vector<float> gQuadNormals{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	const Vec3 gAABBMax(1.0f, 0.0f, 1.0f), gAABBMin(-1.0f, 0.0f, -1.0f);

	const std::vector<uint16_t> gQuadIndices{ 0, 1, 2, 2, 3, 0 };

	enum VSInputLayout
	{
		POSITION = 0,
		NORMAL,
		NUM_INPUT_LAYOUTS
	};


	DX11TerrainPass::DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const RenderSettings::Tesselation& tessData) :
		mContext(context),
		mLayout(nullptr),
		mVertexShader(nullptr),
		mHullShader(nullptr),
		mDomainShader(nullptr),
		mPixelShader(nullptr),

		mCBuffer(device, context, mCBuffer.CONSTANT_BUFFER_SLOT_VERTEX),
		mPerTerrainCBuffer(device, context, mPerTerrainCBuffer.CONSTANT_BUFFER_SLOT_EXTRA),
		mTransformsBuffer(device, context),
		mQuadMesh(device, context, gQuadVertices, gQuadNormals, std::vector<float>(), std::vector<float>(), gQuadIndices, gAABBMin, gAABBMax),
		mVertexTransformer(vertexTransformer),

		mTessData(tessData)
	{
		// input layout
		// TODO: generalize into one class
		D3D11_INPUT_ELEMENT_DESC inputDescription[NUM_INPUT_LAYOUTS];
		ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC) * NUM_INPUT_LAYOUTS);

		inputDescription[VSInputLayout::POSITION].SemanticName = "POSITION";
		inputDescription[VSInputLayout::POSITION].SemanticIndex = 0;
		inputDescription[VSInputLayout::POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescription[VSInputLayout::POSITION].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_POSITIONS;
		inputDescription[VSInputLayout::POSITION].AlignedByteOffset = 0;
		inputDescription[VSInputLayout::POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription[VSInputLayout::POSITION].InstanceDataStepRate = 0;

		inputDescription[VSInputLayout::NORMAL].SemanticName = "NORMAL";
		inputDescription[VSInputLayout::NORMAL].SemanticIndex = 0;
		inputDescription[VSInputLayout::NORMAL].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescription[VSInputLayout::NORMAL].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS;
		inputDescription[VSInputLayout::NORMAL].AlignedByteOffset = 0;
		inputDescription[VSInputLayout::NORMAL].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription[VSInputLayout::NORMAL].InstanceDataStepRate = 0;
		DXCALL(device->CreateInputLayout(inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS, gTerrainVertexShader, sizeof(gTerrainVertexShader), &mLayout));

		// shaders
		DXCALL(device->CreateVertexShader(gTerrainVertexShader, sizeof(gTerrainVertexShader), nullptr, &mVertexShader));
		DXCALL(device->CreateHullShader(gTerrainHullShader, sizeof(gTerrainHullShader), nullptr, &mHullShader));
		DXCALL(device->CreateDomainShader(gTerrainDomainShader, sizeof(gTerrainDomainShader), nullptr, &mDomainShader));
		DXCALL(device->CreatePixelShader(gTerrainPixelShader, sizeof(gTerrainPixelShader), nullptr, &mPixelShader));

		// patch translation transforms are static and used regardless of patchsize
		size_t currentTransformIndex = 0;
		int32_t halfGridHeight = GRID_HEIGHT_IN_PATCHES / 2;
		for (int32_t colNum = -halfGridHeight; colNum < halfGridHeight; ++colNum)
		{
			int32_t halfGridWidth = GRID_WIDTH_IN_PATCHES / 2;
			for (int32_t rowNum = -halfGridWidth; rowNum < halfGridWidth; ++rowNum)
			{
				mPatchTranslationTransforms.at(currentTransformIndex) = glm::translate(Vec3(rowNum, 0.0f, colNum));
				++currentTransformIndex;
			}
		}

	}

	DX11TerrainPass::~DX11TerrainPass()
	{
	}


	void DX11TerrainPass::Render(DX11Texture& heightmap, const Mat4& worldTransform, const float heightScale, const float patchSize, const Mat4& view, const Mat4& viewProjection)
	{
		const Vec4& terrainWorldCenter = worldTransform[3];
		const Vec4 terrainWorldExtent = terrainWorldCenter + (patchSize * GRID_SIZE / 2);
		mPerTerrainCBuffer.SetData({ viewProjection, view, heightScale, -terrainWorldExtent.x, -terrainWorldExtent.z, terrainWorldExtent.x, terrainWorldExtent.z});

		heightmap.BindAsShaderResource(SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA);

		// concatenate all relevant transforms and buffer them to GPU before instanced rendering
		float patchExtent = patchSize / 2;
		Mat4 patchScaleTransform = glm::scale(Vec3(patchExtent, 1.0f, patchExtent));
		for (size_t transformIndex = 0; transformIndex < GRID_SIZE; ++transformIndex)
		{
			Mat4& transform = mTempPatchTransformBuffer[transformIndex];
			transform = worldTransform * patchScaleTransform * mPatchTranslationTransforms.at(transformIndex);
			transform = glm::transpose(transform);
		}

		auto dataBegin = &mTempPatchTransformBuffer.front();
		std::size_t sizeInBytes = mTempPatchTransformBuffer.size() * sizeof(Mat4);
		mTransformsBuffer.SetData(dataBegin, sizeInBytes);
		mTransformsBuffer.Bind(DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA);

		mQuadMesh.DrawInstanced(GRID_SIZE);
	}

	void DX11TerrainPass::BindForRendering()
	{
		mContext->VSSetShader(mVertexShader, nullptr, 0);
		mContext->PSSetShader(mPixelShader, nullptr, 0);
		mContext->IASetInputLayout(mLayout);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		mCBuffer.SetData({ mTessData.mMinDistance, mTessData.mMaxDistance, mTessData.mMinFactor, mTessData.mMaxFactor });
	}
}