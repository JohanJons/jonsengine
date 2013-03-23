#include "include/AssetImporter.h"

#include "include/Resources/JonsPackage.h"

#include "boost/foreach.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

namespace JonsAssetImporter
{
    std::string ParseCommands(const std::vector<std::string>& cmds)
    {
        std::string ret;

        if (cmds.size() > 0)
        {
            // command == IMPORT
            if (cmds.front().compare("import") == 0 && cmds.size() >= 2) 
            {
                ImportFlag flag(NONE);
                Assimp::Importer importer;
                bool flagSet = false;
                std::vector<std::string> assets;
                std::vector<std::string> assetNames;
                std::string package;

                BOOST_FOREACH(const std::string& cmd, cmds)
                {
                    if (cmd.compare("-a") == 0) 
                    {
                        flag = ASSET;
                        flagSet = true;
                    }
                    if (cmd.compare("-n") == 0)
                    {
                        flag = ASSET_NAME;
                        flagSet = true;
                    }
                    if (cmd.compare("-p") == 0)
                    {
                        flag = PACKAGE;
                        flagSet = true;
                    }

                    if (!flagSet)
                    {
                        switch (flag)
                        {
                            case ASSET:         assets.push_back(cmd); break;
                            case ASSET_NAME:    assetNames.push_back(cmd); break;
                            case PACKAGE:       package = cmd; break;
                            default:            break;
                        }
                    }

                    flagSet = false;
                }

                if (assets.size() > 0 && !package.empty())
                    if (Import(package, assets, assetNames, importer))
                        ret = "-JonsAssetImporter: Import succesfull";
                    else 
                    {
                        ret = "-JonsAssetImporter: Import failed: ";
                        ret.append(importer.GetErrorString());
                    }
                else
                    ret = "-JonsAssetImporter: No package name supplied";
            }
            else
                ret = "-JonsAssetImporter: ERROR: Unknown command";
        }
        else 
            ret = "-JonsAssetImporter: ERROR: No commands given";
        
        return ret;
    }


    bool Import(const std::string& packageName, const std::vector<std::string>& assets, const std::vector<std::string>& assetNames, Assimp::Importer importer)
    {
        JonsEngine::JonsPackagePtr pkg = JonsEngine::ReadJonsPkg(packageName);
        if (!pkg)
            pkg = JonsEngine::JonsPackagePtr(new JonsEngine::JonsPackage());
         
        std::vector<std::string>::const_iterator assetName = assetNames.begin();
        BOOST_FOREACH(const std::string& asset, assets)
        {
            const aiScene* scene = importer.ReadFile(asset, aiProcess_CalcTangentSpace      |
                                                            aiProcess_GenNormals            |
                                                            aiProcess_Triangulate           |
                                                            aiProcess_JoinIdenticalVertices |
                                                            aiProcess_SortByPType);
            if(!scene)
                return false;

            ProcessScene(scene, assetName != assetNames.end() ? *assetName : std::string(scene->mRootNode->mName.C_Str()), pkg);

            if (assetName != assetNames.end())
                assetName++;
        }

        JonsEngine::WriteJonsPkg(packageName, pkg);

        return true;
    }

    void ProcessScene(const aiScene* scene, const std::string& modelName, JonsEngine::JonsPackagePtr pkg)
    {
        JonsEngine::PackageModel rootModel(ProcessModel(scene, scene->mRootNode));
        rootModel.mName = modelName;

        pkg->mModels.push_back(rootModel);
    }

    JonsEngine::PackageModel ProcessModel(const aiScene* scene, const aiNode* node)
    {
        JonsEngine::PackageModel model;
        model.mName = node->mName.C_Str();
        model.mTransform = aiMat4ToJonsMat4(node->mTransformation);
        
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            JonsEngine::PackageMesh mesh;
            aiMesh* m = scene->mMeshes[node->mMeshes[i]];
            
            for (unsigned int j = 0; j < m->mNumVertices; j++)
            {
                mesh.mVertexData.push_back(m->mVertices[j].x);
                mesh.mVertexData.push_back(m->mVertices[j].y);
                mesh.mVertexData.push_back(m->mVertices[j].z);
            }

            for (unsigned int j = 0; j < m->mNumVertices; j++)
            {
                mesh.mNormalData.push_back(m->mNormals[j].x);
                mesh.mNormalData.push_back(m->mNormals[j].y);
                mesh.mNormalData.push_back(m->mNormals[j].z);
            }
            
            for (unsigned int j = 0; j < m->mNumFaces; j++)
                mesh.mIndiceData.insert(mesh.mIndiceData.end(), m->mFaces[j].mIndices, m->mFaces[j].mIndices + m->mFaces[j].mNumIndices);


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