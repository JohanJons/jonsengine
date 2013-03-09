#include "include/AssetImporter.h"

#include "include/Resources/JonsPackage.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

namespace JonsAssetImporter
{
    std::string ParseCommands(const std::vector<std::string>& cmds)
    {
        if (cmds.size() > 0)
        {
            if (!cmds.front().compare("import") && cmds.size() >= 3)
            {
                Assimp::Importer importer;

                if (Import(*(cmds.cbegin()+1), cmds.cbegin()+2, cmds.cend(), importer))
                    return "-JonsAssetImporter: Import succesfull";
                else {
                    std::string error = "-JonsAssetImporter: Import failed: ";
                    error.append(importer.GetErrorString());

                    return error;
                }
            }
        }

        return "-JonsAssetImporter: ERROR: No commands given";
    }

    bool Import(const std::string& jonsPkgName, std::vector<std::string>::const_iterator assetsCurrent, std::vector<std::string>::const_iterator assetsEnd, Assimp::Importer importer)
    {
        JonsEngine::JonsPackagePtr pkg = JonsEngine::ReadJonsPkg(jonsPkgName);
        if (!pkg)
            pkg = JonsEngine::JonsPackagePtr(new JonsEngine::JonsPackage);
         
        while (assetsCurrent != assetsEnd)
        {
            const aiScene* scene = importer.ReadFile(*assetsCurrent, aiProcess_CalcTangentSpace      | 
                                                                     aiProcess_Triangulate           |
                                                                     aiProcess_JoinIdenticalVertices |
                                                                     aiProcess_SortByPType);
            if(!scene)
                return false;

            ProcessScene(scene, pkg);

            assetsCurrent++;
        }

        JonsEngine::WriteJonsPkg(jonsPkgName, pkg);

        return true;
    }

    void ProcessScene(const aiScene* scene, JonsEngine::JonsPackagePtr pkg)
    {
        for(unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++)
        {
            JonsEngine::PackageModel model = ProcessModel(scene, scene->mRootNode->mChildren[i]);

            pkg->mModels.push_back(model);
        }

    }

    /*void ProcessMeshes(const aiScene* scene, JonsEngine::JonsPackagePtr pkg)
    {
        for(unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            JonsEngine::PackageMesh mesh;
            aiMesh* m = scene->mMeshes[i];
            
            for (unsigned int j = 0; j < m->mNumFaces; j++)
                mesh.mIndiceData.insert(mesh.mIndiceData.end(), m->mFaces[j].mIndices, m->mFaces[j].mIndices + m->mFaces[j].mNumIndices);
            
            mesh.mVertexData.resize(m->mNumVertices * sizeof(float));
            for (unsigned int j = 0; j < m->mNumVertices; j++)
            {
                mesh.mVertexData.push_back(m->mVertices[j].x);
                mesh.mVertexData.push_back(m->mVertices[j].y);
                mesh.mVertexData.push_back(m->mVertices[j].z);
            }

            pkg->mMeshes.push_back(mesh);
        }
    }*/

    JonsEngine::PackageModel ProcessModel(const aiScene* scene, const aiNode* node)
    {
        JonsEngine::PackageModel model;
        model.mName = node->mName.C_Str();
        model.mTransform = aiMat4ToJonsMat4(node->mTransformation);
        
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            JonsEngine::PackageMesh mesh;
            aiMesh* m = scene->mMeshes[node->mMeshes[i]];
            
            for (unsigned int j = 0; j < m->mNumFaces; j++)
                mesh.mIndiceData.insert(mesh.mIndiceData.end(), m->mFaces[j].mIndices, m->mFaces[j].mIndices + m->mFaces[j].mNumIndices);
            
            for (unsigned int j = 0; j < m->mNumVertices; j++)
            {
                mesh.mVertexData.push_back(m->mVertices[j].x);
                mesh.mVertexData.push_back(m->mVertices[j].y);
                mesh.mVertexData.push_back(m->mVertices[j].z);
            }

            model.mMeshes.push_back(mesh);
        }
        
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            model.mChildren.push_back(ProcessModel(scene, node->mChildren[i]));

        return model;
    }

    JonsEngine::Mat4 aiMat4ToJonsMat4(aiMatrix4x4 aiMat)
    {
        JonsEngine::Mat4 jMat;

        jMat[0].x = aiMat.a1; jMat[0].y = aiMat.a2; jMat[0].z = aiMat.a3; jMat[0].w = aiMat.a4;
        jMat[1].x = aiMat.b1; jMat[1].y = aiMat.b2; jMat[1].z = aiMat.b3; jMat[1].w = aiMat.b4;
        jMat[2].x = aiMat.c1; jMat[2].y = aiMat.c2; jMat[2].z = aiMat.c3; jMat[2].w = aiMat.c4;
        jMat[3].x = aiMat.d1; jMat[3].y = aiMat.d2; jMat[3].z = aiMat.d3; jMat[3].w = aiMat.d4;

        return jMat;
    }
}