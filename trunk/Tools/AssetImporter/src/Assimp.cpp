#include "include/Assimp.h"

#include "include/Core/Utils/Math.h"
#include "include/FreeImage.h"

#include <limits>

using namespace JonsEngine;

namespace JonsAssetImporter
{
    JonsEngine::Mat4 aiMat4ToJonsMat4(const aiMatrix4x4& aiMat)
    {
        Mat4 jMat;

        jMat[0][0] = aiMat.a1; jMat[0][1] = aiMat.b1; jMat[0][2] = aiMat.c1; jMat[0][3] = aiMat.d1;
        jMat[1][0] = aiMat.a2; jMat[1][1] = aiMat.b2; jMat[1][2] = aiMat.c2; jMat[1][3] = aiMat.d2;
        jMat[2][0] = aiMat.a3; jMat[2][1] = aiMat.b3; jMat[2][2] = aiMat.c3; jMat[2][3] = aiMat.d3;
        jMat[3][0] = aiMat.a4; jMat[3][1] = aiMat.b4; jMat[3][2] = aiMat.c4; jMat[3][3] = aiMat.d4;

        return jMat;
    }

    JonsEngine::Vec3 aiColor3DToJonsVec3(const aiColor3D& color)
    {
        return Vec3(color.r, color.g, color.b);
    }

    JonsEngine::Vec3 aiVec3ToJonsVec3(const aiVector3D& vec)
    {
        return Vec3(vec.x, vec.y, vec.z);
    }


    Assimp::Assimp()
    {
    }

    Assimp::~Assimp()
    {
    }


    bool Assimp::ProcessScene(const boost::filesystem::path& modelPath, const std::string& modelName, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg)
    {
        const aiScene* scene = mImporter.ReadFile(modelPath.string(), aiProcessPreset_TargetRealtime_MaxQuality);
        if (!scene)
        {
            Log("-JonsAssetImporter: Assimp parsing error: ");
            Log(mImporter.GetErrorString());
            return false;
        }

        // process materials
        // map scene material indexes to actual package material indexes
        MaterialMap materialMap;
        ProcessAssimpMaterials(scene, modelPath, materialMap, freeimageImporter, pkg);

        // process model hierarchy
        PackageModel rootModel(ProcessAssimpModelGeometry(scene, scene->mRootNode, materialMap));
        rootModel.mName = modelName;
        pkg->mModels.push_back(rootModel);

        return true;
    }

    void Assimp::ProcessAssimpMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg)
    {
        if (!scene->HasMaterials())
            return;

        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            const aiMaterial* material = scene->mMaterials[i];
            PackageMaterial pkgMaterial;
            aiString assimpTexturePath;

            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &assimpTexturePath) == aiReturn_SUCCESS)
            {
                const boost::filesystem::path texturePath = std::string(assimpTexturePath.C_Str());

                std::string fullTexturePath = "";
                if (modelPath.has_parent_path())
                {
                    fullTexturePath.append(modelPath.parent_path().string());
                    fullTexturePath.append("/");
                }
                fullTexturePath.append(texturePath.string());

                PackageTexture diffuseTexture = freeimageImporter.ProcessTexture(fullTexturePath, TEXTURE_TYPE_DIFFUSE);
                pkgMaterial.mDiffuseTexture = diffuseTexture;
                pkgMaterial.mHasDiffuseTexture = true;
            }

            if ((material->GetTextureCount(aiTextureType_NORMALS) > 0 && material->GetTexture(aiTextureType_NORMALS, 0, &assimpTexturePath) == aiReturn_SUCCESS) ||
                (material->GetTextureCount(aiTextureType_HEIGHT) > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &assimpTexturePath) == aiReturn_SUCCESS))
            {
                const boost::filesystem::path texturePath = std::string(assimpTexturePath.C_Str());

                std::string fullTexturePath = "";
                if (modelPath.has_parent_path())
                {
                    fullTexturePath.append(modelPath.parent_path().string());
                    fullTexturePath.append("/");
                }
                fullTexturePath.append(texturePath.string());

                PackageTexture normalTexture = freeimageImporter.ProcessTexture(fullTexturePath, TEXTURE_TYPE_NORMAL);
                pkgMaterial.mNormalTexture = normalTexture;
                pkgMaterial.mHasNormalTexture = true;
            }

            aiString materialName;
            aiColor3D diffuseColor;
            aiColor3D ambientColor(0.1f);
            aiColor3D specularColor;
            aiColor3D emissiveColor;

            material->Get(AI_MATKEY_NAME, materialName);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            //if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS)     TODO
            //    ambientColor = aiColor3D(1.0f);
            material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
            material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

            pkgMaterial.mName = materialName.C_Str();
            pkgMaterial.mDiffuseColor = aiColor3DToJonsVec3(diffuseColor);
            pkgMaterial.mAmbientColor = aiColor3DToJonsVec3(ambientColor);
            pkgMaterial.mSpecularColor = aiColor3DToJonsVec3(specularColor);
            pkgMaterial.mEmissiveColor = aiColor3DToJonsVec3(emissiveColor);

            std::vector<PackageMaterial>::iterator iter = pkg->mMaterials.insert(pkg->mMaterials.end(), pkgMaterial);

            materialMap.insert(MaterialPair(i, std::distance(pkg->mMaterials.begin(), iter)));
        }
    }

    JonsEngine::PackageModel Assimp::ProcessAssimpModelGeometry(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap)
    {
        PackageModel model;
        model.mName = node->mName.C_Str();
        model.mTransform = aiMat4ToJonsMat4(node->mTransformation);

        // assume root node has no meshes
        assert(scene->mRootNode->mMeshes == nullptr);

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            model.mNodes.emplace_back(ProcessAssimpNode(scene, node->mChildren[i], materialMap, model.mAABB.mMinBounds, model.mAABB.mMaxBounds));

        return model;
    }

    JonsEngine::PackageNode Assimp::ProcessAssimpNode(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap, JonsEngine::Vec3& parentMinBounds, JonsEngine::Vec3& parentMaxBounds)
    {
        PackageNode pkgNode;
        pkgNode.mName = node->mName.C_Str();
        pkgNode.mTransform = aiMat4ToJonsMat4(node->mTransformation);

        for (uint32_t i = 0; i < node->mNumMeshes; i++)
            pkgNode.mMeshes.emplace_back(ProcessAssimpMesh(scene->mMeshes[node->mMeshes[i]], materialMap, pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds));

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            pkgNode.mChildNodes.emplace_back(ProcessAssimpNode(scene, node->mChildren[i], materialMap, pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds));

        parentMinBounds = MinVal(parentMinBounds, pkgNode.mAABB.mMinBounds);
        parentMaxBounds = MaxVal(parentMaxBounds, pkgNode.mAABB.mMaxBounds);

        return pkgNode;
    }

    JonsEngine::PackageMesh Assimp::ProcessAssimpMesh(const aiMesh* m, const MaterialMap& materialMap, JonsEngine::Vec3& nodeMinBounds, JonsEngine::Vec3& nodeMaxBounds)
    {
        PackageMesh pkgMesh;
        pkgMesh.mName = m->mName.C_Str();

        pkgMesh.mVertexData.reserve(m->mNumVertices * 3);
        pkgMesh.mNormalData.reserve(m->mNumVertices * 3);
        pkgMesh.mTexCoordsData.reserve(m->mNumVertices * 2);
        pkgMesh.mTangents.reserve(m->mNumVertices * 3);
        pkgMesh.mBitangents.reserve(m->mNumVertices * 3);

        // vertice, normal, texcoord, tangents and bitangents data
        for (unsigned int j = 0; j < m->mNumVertices; j++)
        {
            pkgMesh.mVertexData.push_back(m->mVertices[j].x);
            pkgMesh.mVertexData.push_back(m->mVertices[j].y);
            pkgMesh.mVertexData.push_back(m->mVertices[j].z);

            if (m->HasNormals())
            {
                pkgMesh.mNormalData.push_back(m->mNormals[j].x);
                pkgMesh.mNormalData.push_back(m->mNormals[j].y);
                pkgMesh.mNormalData.push_back(m->mNormals[j].z);
            }

            if (m->HasTextureCoords(0))
            {
                pkgMesh.mTexCoordsData.push_back(m->mTextureCoords[0][j].x);
                pkgMesh.mTexCoordsData.push_back(m->mTextureCoords[0][j].y);
            }

            if (m->HasTangentsAndBitangents())
            {
                pkgMesh.mTangents.push_back(m->mTangents[j].x);
                pkgMesh.mTangents.push_back(m->mTangents[j].y);
                pkgMesh.mTangents.push_back(m->mTangents[j].z);

                pkgMesh.mBitangents.push_back(m->mBitangents[j].x);
                pkgMesh.mBitangents.push_back(m->mBitangents[j].y);
                pkgMesh.mBitangents.push_back(m->mBitangents[j].z);
            }

            // mesh AABB
            const Vec3 vertex = aiVec3ToJonsVec3(m->mVertices[j]);
            pkgMesh.mAABB.mMinBounds = MinVal(pkgMesh.mAABB.mMinBounds, vertex);
            pkgMesh.mAABB.mMaxBounds = MaxVal(pkgMesh.mAABB.mMaxBounds, vertex);
        }

        pkgMesh.mIndiceData.reserve(m->mNumFaces * 3);

        // index data
        for (uint32_t j = 0; j < m->mNumFaces; j++)
        {
            // only dem triangles
            assert(m->mFaces[j].mNumIndices == 3);
            for (uint32_t index = 0; index < 3; index++)
            {
                assert(m->mFaces[j].mIndices[index] <= UINT16_MAX);
                pkgMesh.mIndiceData.push_back(m->mFaces[j].mIndices[index]);
            }
        }

        if (materialMap.find(m->mMaterialIndex) != materialMap.end())
        {
            pkgMesh.mMaterialIndex = materialMap.at(m->mMaterialIndex);
            pkgMesh.mHasMaterial = true;
        }

        // node AABB
        nodeMinBounds = MinVal(nodeMinBounds, pkgMesh.mAABB.mMinBounds);
        nodeMaxBounds = MaxVal(nodeMaxBounds, pkgMesh.mAABB.mMaxBounds);

        return pkgMesh;
    }
}