#include "include/Scene/Mesh.h"

#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    Mesh::Mesh(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material) :
        mName(name), mMaterialTilingFactor(1.0f), mAABB(0.5f * (minBounds + maxBounds), 0.5f * (maxBounds - minBounds)), mMeshID(meshID), mMaterial(material)
    {
    }

    Mesh::~Mesh()
    {
    }


	float Mesh::GetTextureTilingFactor() const
	{
		return mMaterialTilingFactor;
	}

    void Mesh::SetTextureTilingFactor(const float tilingFactor)
	{
		mMaterialTilingFactor = tilingFactor;
	}


	MaterialPtr Mesh::GetMaterial() const
	{
		return mMaterial;
	}

	void Mesh::SetMaterial(MaterialPtr material)
	{
		mMaterial = material;
	}
}