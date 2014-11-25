#include "include/Assimp.h"

#include "include/FreeImage.h"

using namespace JonsEngine;

namespace JonsAssetImporter
{
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
        PackageModel rootModel(ProcessAssimpModelGeometry(scene, scene->mRootNode, materialMap, Vec3(0.0f), Vec3(0.0f)));
        rootModel.mName = modelName;
        pkg->mModels.push_back(rootModel);

        return true;
    }

    void Assimp::ProcessAssimpMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg)
    {
        if (!scene->HasMaterials())
            return;

        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
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

    JonsEngine::PackageModel Assimp::ProcessAssimpModelGeometry(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap, Vec3& modelMinBounds, Vec3& modelMaxBounds)
    {
        PackageModel model;
        model.mName = node->mName.C_Str();
        model.mTransform = aiMat4ToJonsMat4(node->mTransformation);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            PackageMesh mesh;
            const aiMesh* m = scene->mMeshes[node->mMeshes[i]];

            mesh.mVertexData.reserve(m->mNumVertices * 3);
            mesh.mNormalData.reserve(m->mNumVertices * 3);
            mesh.mTexCoordsData.reserve(m->mNumVertices * 2);
            mesh.mTangents.reserve(m->mNumVertices * 3);
            mesh.mBitangents.reserve(m->mNumVertices * 3);

            // vertice, normal, texcoord, tangents and bitangents data
            for (unsigned int j = 0; j < m->mNumVertices; j++)
            {
                mesh.mVertexData.push_back(m->mVertices[j].x);
                mesh.mVertexData.push_back(m->mVertices[j].y);
                mesh.mVertexData.push_back(m->mVertices[j].z);

                if (m->HasNormals())
                {
                    mesh.mNormalData.push_back(m->mNormals[j].x);
                    mesh.mNormalData.push_back(m->mNormals[j].y);
                    mesh.mNormalData.push_back(m->mNormals[j].z);
                }
                
                if (m->HasTextureCoords(0))
                {
                    mesh.mTexCoordsData.push_back(m->mTextureCoords[0][j].x);
                    mesh.mTexCoordsData.push_back(m->mTextureCoords[0][j].y);
                }

                if (m->HasTangentsAndBitangents())
                {
                    mesh.mTangents.push_back(m->mTangents[j].x);
                    mesh.mTangents.push_back(m->mTangents[j].y);
                    mesh.mTangents.push_back(m->mTangents[j].z);

                    mesh.mBitangents.push_back(m->mBitangents[j].x);
                    mesh.mBitangents.push_back(m->mBitangents[j].y);
                    mesh.mBitangents.push_back(m->mBitangents[j].z);
                }

                // mesh AABB
                if (m->mVertices[j].x > mesh.mAABB.mMaxBounds.x) mesh.mAABB.mMaxBounds.x = m->mVertices[j].x;
                if (m->mVertices[j].x < mesh.mAABB.mMinBounds.x) mesh.mAABB.mMinBounds.x = m->mVertices[j].x;
                if (m->mVertices[j].y > mesh.mAABB.mMaxBounds.y) mesh.mAABB.mMaxBounds.y = m->mVertices[j].y;
                if (m->mVertices[j].y < mesh.mAABB.mMinBounds.y) mesh.mAABB.mMinBounds.y = m->mVertices[j].y;
                if (m->mVertices[j].z > mesh.mAABB.mMaxBounds.z) mesh.mAABB.mMaxBounds.z = m->mVertices[j].z;
                if (m->mVertices[j].z < mesh.mAABB.mMinBounds.z) mesh.mAABB.mMinBounds.z = m->mVertices[j].z;
            }

            mesh.mIndiceData.reserve(m->mNumFaces * 3);

            // index data
            for (unsigned int j = 0; j < m->mNumFaces; j++)
            {
                // only dem triangles
                assert(m->mFaces[j].mNumIndices == 3);
                for (uint32_t index = 0; index < 3; index++)
                {
                    assert(m->mFaces[j].mIndices[index] <= UINT16_MAX);
                    mesh.mIndiceData.push_back(m->mFaces[j].mIndices[index]);
                }
            }

            if (materialMap.find(m->mMaterialIndex) != materialMap.end())
            {
                mesh.mMaterialIndex = materialMap.at(m->mMaterialIndex);
                mesh.mHasMaterial = true;
            }

            // model AABB
            if (mesh.mAABB.mMaxBounds.x > modelMaxBounds.x) modelMaxBounds.x = mesh.mAABB.mMaxBounds.x;
            if (mesh.mAABB.mMinBounds.x < modelMinBounds.x) modelMinBounds.x = mesh.mAABB.mMinBounds.x;
            if (mesh.mAABB.mMaxBounds.y > modelMaxBounds.y) modelMaxBounds.y = mesh.mAABB.mMaxBounds.y;
            if (mesh.mAABB.mMinBounds.y < modelMinBounds.y) modelMinBounds.y = mesh.mAABB.mMinBounds.y;
            if (mesh.mAABB.mMaxBounds.z > modelMaxBounds.z) modelMaxBounds.z = mesh.mAABB.mMaxBounds.z;
            if (mesh.mAABB.mMinBounds.z < modelMinBounds.z) modelMinBounds.z = mesh.mAABB.mMinBounds.z;

            // add mesh to collection
            model.mMeshes.push_back(mesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            model.mChildren.push_back(ProcessAssimpModelGeometry(scene, node->mChildren[i], materialMap, modelMinBounds, modelMaxBounds));

        model.mAABB.mMinBounds = modelMinBounds;
        model.mAABB.mMaxBounds = modelMaxBounds;

        return model;
    }


    JonsEngine::Mat4 Assimp::aiMat4ToJonsMat4(const aiMatrix4x4& aiMat)
    {
        Mat4 jMat;

        jMat[0][0] = aiMat.a1; jMat[0][1] = aiMat.b1; jMat[0][2] = aiMat.c1; jMat[0][3] = aiMat.d1;
        jMat[1][0] = aiMat.a2; jMat[1][1] = aiMat.b2; jMat[1][2] = aiMat.c2; jMat[1][3] = aiMat.d2;
        jMat[2][0] = aiMat.a3; jMat[2][1] = aiMat.b3; jMat[2][2] = aiMat.c3; jMat[2][3] = aiMat.d3;
        jMat[3][0] = aiMat.a4; jMat[3][1] = aiMat.b4; jMat[3][2] = aiMat.c4; jMat[3][3] = aiMat.d4;

        return jMat;
    }

    JonsEngine::Vec3 Assimp::aiColor3DToJonsVec3(const aiColor3D& color)
    {
        return Vec3(color.r, color.g, color.b);
    }
}