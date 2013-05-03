#include "include/AssetImporter.h"

#include "include/Resources/JonsPackage.h"

#include "boost/foreach.hpp"
#include "boost/bind.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

using namespace JonsEngine;

namespace JonsAssetImporter
{
    static AssetImporter* gInstance = NULL;

    AssetImporter::AssetImporter()
    {
        FreeImage_Initialise(false);
        FreeImage_SetOutputMessage(FreeImageErrorHandler);
        gInstance = this;
    }

    AssetImporter::~AssetImporter()
    {
        FreeImage_DeInitialise();
        FreeImage_SetOutputMessage(NULL);
        gInstance = NULL;
    }

    AssetImporter::ParseCmdResult AssetImporter::ParseCommands(const std::vector<std::string>& cmds)
    {
        ParseCmdResult ret = FAIL;

        if (cmds.size() > 0)
        {
            // command == IMPORT
            if (cmds.front().compare("import") == 0 && cmds.size() >= 2) 
            {
                ImportFlag flag(NONE);
                Assimp::Importer importer;
                bool flagSet = false;
                std::vector<boost::filesystem::path> assets;
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
                            case PACKAGE:       package = cmd;  if (package.size() <= 5 || (package.compare(package.size() - 5, 5, ".jons") != 0)) package.append(".jons");  break;
                            default:            break;
                        }
                    }

                    flagSet = false;
                }

                if (assets.size() <= 0)
                    Log("-JonsAssetImporter: ERROR: No assets supplied");
                else if (package.empty())
                    Log("-JonsAssetImporter: ERROR: No package name given");
                // Try import of assets
                else if (Import(package, assets, assetNames, importer))
                    ret = SUCCESS;
                else {
                    Log("-JonsAssetImporter: Assimp parsing error: ");
                    Log(importer.GetErrorString());
                }

            }
            else
                Log("-JonsAssetImporter: ERROR: Unknown command");
        }
        else 
            Log("-JonsAssetImporter: ERROR: No commands given");
        
        return ret;
    }


    const std::string& AssetImporter::GetErrorLog() const
    {
        return mErrorLog;
    }


    bool AssetImporter::Import(const std::string& packageName, const std::vector<boost::filesystem::path>& assets, const std::vector<std::string>& assetNames, Assimp::Importer importer)
    {
        JonsPackagePtr pkg;// = ReadJonsPkg(packageName);   // TODO: support opening previous package
        if (!pkg)
            pkg = JonsPackagePtr(new JonsPackage());
         
        std::vector<std::string>::const_iterator assetName = assetNames.begin();
        BOOST_FOREACH(const boost::filesystem::path& asset, assets)
        {
            if (boost::filesystem::exists(asset) && boost::filesystem::is_regular_file(asset))
            {
                const aiScene* scene = importer.ReadFile(asset.string(), aiProcessPreset_TargetRealtime_MaxQuality);
                if(!scene)
                    return false;

                ProcessScene(scene, asset, assetName != assetNames.end() ? *assetName : std::string(scene->mRootNode->mName.C_Str()), pkg);
            }

            if (assetName != assetNames.end())
                assetName++;
        }

        WriteJonsPkg(packageName, pkg);

        return true;
    }

    void AssetImporter::ProcessScene(const aiScene* scene, const boost::filesystem::path& modelPath, const std::string& modelName, JonsPackagePtr pkg)
    {
        // process materials
        MaterialMap materialMap;      // map scene material indexes to actual package material indexes
        ProcessMaterials(scene, modelPath, materialMap, pkg);
        
        // process model hierarchy
        PackageModel rootModel(ProcessModel(scene, scene->mRootNode, materialMap));
        rootModel.mName = modelName;
        pkg->mModels.push_back(rootModel);
    }


    void AssetImporter::ProcessMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, JonsPackagePtr pkg)
    {
        if (scene->HasMaterials())
        {
            for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            {
                const aiMaterial* material = scene->mMaterials[i];
                aiString texturePath;

                if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) 
                {
                    const char* textureName = texturePath.C_Str();

                    FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(textureName);
                    if (imageFormat == FIF_UNKNOWN) 
                        imageFormat = FreeImage_GetFIFFromFilename(textureName);

                    if (imageFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(imageFormat))     // if still unknown or unsupported, move on
                        continue;

                    std::string filePath = modelPath.parent_path().string();
                    if (modelPath.has_parent_path())
                        filePath.append("/");
                    filePath.append(textureName);
                    FIBITMAP* bitmap = FreeImage_Load(imageFormat, filePath.c_str());

                    if (!bitmap || !FreeImage_GetBits(bitmap) || !FreeImage_GetWidth(bitmap) || !FreeImage_GetHeight(bitmap))
                        continue;

                    FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
                    uint32_t bitsPerPixel           = FreeImage_GetBPP(bitmap);
                    uint32_t widthInPixels          = FreeImage_GetWidth(bitmap);
                    uint32_t heightInPixels         = FreeImage_GetHeight(bitmap);

                    // should be OK now
                    PackageMaterial pkgMaterial;
                    pkgMaterial.mName             = textureName;
                    pkgMaterial.mBitsPerPixel     = bitsPerPixel;
                    pkgMaterial.mTextureWidth     = widthInPixels;
                    pkgMaterial.mTextureHeight    = heightInPixels;
                    pkgMaterial.mTextureColorType = colorType == FIC_RGB ? PackageMaterial::RGB : colorType == FIC_RGBALPHA ? PackageMaterial::RGBA : PackageMaterial::UNKNOWN;
                    pkgMaterial.mTextureData.insert(pkgMaterial.mTextureData.begin(), FreeImage_GetBits(bitmap), FreeImage_GetBits(bitmap) + ((bitsPerPixel/8) * widthInPixels * heightInPixels));
                    std::vector<PackageMaterial>::iterator iter = pkg->mMaterials.insert(pkg->mMaterials.end(), pkgMaterial);

                    materialMap.insert(MaterialPair(i, std::distance(pkg->mMaterials.begin(), iter)));

                    FreeImage_Unload(bitmap);
                }
            }
        }
    }

    PackageModel AssetImporter::ProcessModel(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap)
    {
        PackageModel model;
        model.mName = node->mName.C_Str();
        model.mTransform = aiMat4ToJonsMat4(node->mTransformation);
        
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            PackageMesh mesh;
            const aiMesh* m = scene->mMeshes[node->mMeshes[i]];
            
            // vertice, normal and texcoord data
            for (unsigned int j = 0; j < m->mNumVertices; j++)
            {
                mesh.mVertexData.push_back(Vec3(m->mVertices[j].x, m->mVertices[j].y, m->mVertices[j].z));
                
                if (m->HasNormals())
                    mesh.mNormalData.push_back(Vec3(m->mNormals[j].x, m->mNormals[j].y, m->mNormals[j].z));

                if (m->HasTextureCoords(0))
                    mesh.mTexCoordsData.push_back(Vec2(m->mTextureCoords[0][j].x, m->mTextureCoords[0][j].y));
            }
            
            // index data
            for (unsigned int j = 0; j < m->mNumFaces; j++)
                mesh.mIndiceData.insert(mesh.mIndiceData.end(), m->mFaces[j].mIndices, m->mFaces[j].mIndices + m->mFaces[j].mNumIndices);

            if (materialMap.find(m->mMaterialIndex) != materialMap.end())
                mesh.mMaterialIndex = materialMap.at(m->mMaterialIndex);

            // add mesh to collection
            model.mMeshes.push_back(mesh);
        }
        
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            model.mChildren.push_back(ProcessModel(scene, node->mChildren[i], materialMap));

        return model;
    }


    Mat4 AssetImporter::aiMat4ToJonsMat4(aiMatrix4x4 aiMat)
    {
        Mat4 jMat;

        jMat[0].x = aiMat.a1; jMat[0].y = aiMat.a2; jMat[0].z = aiMat.a3; jMat[0].w = aiMat.a4;
        jMat[1].x = aiMat.b1; jMat[1].y = aiMat.b2; jMat[1].z = aiMat.b3; jMat[1].w = aiMat.b4;
        jMat[2].x = aiMat.c1; jMat[2].y = aiMat.c2; jMat[2].z = aiMat.c3; jMat[2].w = aiMat.c4;
        jMat[3].x = aiMat.d1; jMat[3].y = aiMat.d2; jMat[3].z = aiMat.d3; jMat[3].w = aiMat.d4;

        return jMat;
    }

    void AssetImporter::Log(const std::string& msg)
    {
        mErrorLog.append(msg); 
        mErrorLog.append("\n");
    }

    void AssetImporter::FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message)
    {
        if (gInstance)
            gInstance->Log(message);
    }
}