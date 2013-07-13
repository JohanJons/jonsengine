#include "include/AssetImporter.h"

#include "include/Resources/JonsPackage.h"
#include "include/Core/EngineDefs.h"

#include "boost/foreach.hpp"
#include "boost/bind.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <sstream>

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


    bool AssetImporter::ParseCommands(const Command command,  std::vector<std::string>& parameters)
    {
        if (parameters.size() <= 0)
        {
            Log("-JonsAssetImporter: ERROR: No commands given");
            return false;
        }

        ImportFlag flag(UNKNOWN_FLAG);
        Assimp::Importer importer;
        std::vector<boost::filesystem::path> assets;
        std::vector<std::string> assetNames;
        std::string package;

        BOOST_FOREACH(const std::string& parameter, parameters)
        {
            if (parameter.compare("-a") == 0)
            {
                flag = ASSET;
                continue;
            }
            else if (parameter.compare("-n") == 0)
            {
                flag = ASSET_NAME;
                continue;
            }
            else if (parameter.compare("-p") == 0)
            {
                flag = PACKAGE;
                continue;
            }

            switch (flag)
            {
                case ASSET:
                    {
                        assets.push_back(parameter);
                        break;
                    }

                case ASSET_NAME:
                    {
                        assetNames.push_back(parameter);
                        break;
                    }

                case PACKAGE:       
                    {
                        if (!package.empty())
                        {
                            Log("-JonsAssetImporter: ERROR: Several package names given");
                            return false;
                        }

                        package = parameter;
                        if (package.size() <= 5 || (package.compare(package.size() - 5, 5, ".jons") != 0))
                            package.append(".jons");
                        break;
                    }

                default:
                    {
                        Log("-JonsAssetImporter: ERROR: No parameter flag set");
                        return false;
                    }
            }
        }

        // verify parameters
        if (assets.size() <= 0) {
            Log("-JonsAssetImporter: ERROR: No assets supplied");
            return false;
        }
        
        if (package.empty()) {
            Log("-JonsAssetImporter: ERROR: No package name given");
            return false;
        }

        switch (command)
        {
            case Command::IMPORT:
                return Import(package, assets, assetNames, importer);

            default:
                {
                    Log("-JonsAssetImporter: ERROR: Unknown command");
                    return false;
                }

        }
    }

    const std::string& AssetImporter::GetErrorLog() const
    {
        return mErrorLog;
    }


    bool AssetImporter::Import(const std::string& packageName, const std::vector<boost::filesystem::path>& assets, const std::vector<std::string>& assetNames, Assimp::Importer importer)
    {
        JonsPackagePtr pkg;// = ReadJonsPkg(packageName);   // TODO: support opening previous package
        uint32_t materialNum = 0;
        if (!pkg)
            pkg = JonsPackagePtr(new JonsPackage());
         
        std::vector<std::string>::const_iterator assetName = assetNames.begin();
        BOOST_FOREACH(const boost::filesystem::path& asset, assets)
        {
            if (!boost::filesystem::exists(asset) || !boost::filesystem::is_regular_file(asset))
                continue;

            switch (GetAssetType(asset.string().c_str()))
            {
                case MODEL:
                    {
                        const aiScene* scene = importer.ReadFile(asset.string(), aiProcessPreset_TargetRealtime_MaxQuality);
                        if(!scene)
                        {
                            Log("-JonsAssetImporter: Assimp parsing error: ");
                            Log(importer.GetErrorString());
                            return false;
                        }

                        ProcessScene(scene, asset, assetName != assetNames.end() ? *assetName : std::string(scene->mRootNode->mName.C_Str()), pkg);

                        break;
                    }

                case MATERIAL:
                    {
                        PackageMaterial material;
                        material.mName           = assetName != assetNames.end() ? *assetName : GetDefaultAssetName(MATERIAL, materialNum++);
                        material.mDiffuseTexture = ProcessDiffuseTexture(asset);
                        pkg->mMaterials.push_back(material);

                        break;
                    }

                default:
                    break;
            }

            if (assetName != assetNames.end())
                assetName++;
        }

        WriteJonsPkg(packageName, pkg);

        return true;
    }

    /*
     * Parses all models in assimp scene 'scene' into 'pkg'
     */
    void AssetImporter::ProcessScene(const aiScene* scene, const boost::filesystem::path& modelPath, const std::string& modelName, JonsPackagePtr pkg)
    {
        // process materials
        MaterialMap materialMap;      // map scene material indexes to actual package material indexes
        ProcessAssimpMaterials(scene, modelPath, materialMap, pkg);
         
        // process model hierarchy
        PackageModel rootModel(ProcessAssimpModelGeometry(scene, scene->mRootNode, materialMap));
        rootModel.mName = modelName;
        pkg->mModels.push_back(rootModel);
    }

    /*
     * Parses all materials in 'scene' into 'pkg' and maps all assimp material indexes to jonspkg materials in 'materialMap'
     */
    void AssetImporter::ProcessAssimpMaterials(const aiScene* scene, const boost::filesystem::path& assetPath, MaterialMap& materialMap, JonsPackagePtr pkg)
    {
        if (!scene->HasMaterials())
            return;

        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            const aiMaterial* material = scene->mMaterials[i];
            PackageMaterial pkgMaterial;

            aiString texturePath;
            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS) 
            {
                std::string fullTexturePath = assetPath.parent_path().string();
                if (assetPath.has_parent_path())
                    fullTexturePath.append("/");
                fullTexturePath.append(texturePath.C_Str());

                PackageTexture diffuseTexture(ProcessDiffuseTexture(fullTexturePath));
                pkgMaterial.mDiffuseTexture = diffuseTexture;
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

            pkgMaterial.mName          = materialName.C_Str();
            pkgMaterial.mDiffuseColor  = aiColor3DToJonsVec3(diffuseColor);
            pkgMaterial.mAmbientColor  = aiColor3DToJonsVec3(ambientColor);
            pkgMaterial.mSpecularColor = aiColor3DToJonsVec3(specularColor);
            pkgMaterial.mEmissiveColor = aiColor3DToJonsVec3(emissiveColor);

            std::vector<PackageMaterial>::iterator iter = pkg->mMaterials.insert(pkg->mMaterials.end(), pkgMaterial);

            materialMap.insert(MaterialPair(i, std::distance(pkg->mMaterials.begin(), iter)));
        }
    }

    /*
     * Parses all model geometry at 'node' and all child models under it
     */
    PackageModel AssetImporter::ProcessAssimpModelGeometry(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap)
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
            }
            
            // index data
            for (unsigned int j = 0; j < m->mNumFaces; j++)
                mesh.mIndiceData.insert(mesh.mIndiceData.end(), m->mFaces[j].mIndices, m->mFaces[j].mIndices + m->mFaces[j].mNumIndices);

            if (materialMap.find(m->mMaterialIndex) != materialMap.end()) 
            {
                mesh.mMaterialIndex = materialMap.at(m->mMaterialIndex);
                mesh.mHasMaterial   = true;
            }

            // add mesh to collection
            model.mMeshes.push_back(mesh);
        }
        
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            model.mChildren.push_back(ProcessAssimpModelGeometry(scene, node->mChildren[i], materialMap));

        return model;
    }

    /*
     * Parses a texture pointed by 'assetPath'
     */
    PackageTexture AssetImporter::ProcessDiffuseTexture(const boost::filesystem::path& assetPath)
    {
        PackageTexture diffuseTexture;
        const std::string filename = assetPath.filename().string();

        FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(filename.c_str());
        if (imageFormat == FIF_UNKNOWN) 
            imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());

        if (imageFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(imageFormat))
            return diffuseTexture;

        FIBITMAP* bitmap = FreeImage_Load(imageFormat, assetPath.string().c_str());

        if (!bitmap || !FreeImage_GetBits(bitmap) || !FreeImage_GetWidth(bitmap) || !FreeImage_GetHeight(bitmap))
            return diffuseTexture;

        FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
        uint32_t bitsPerPixel           = FreeImage_GetBPP(bitmap);
        uint32_t widthInPixels          = FreeImage_GetWidth(bitmap);
        uint32_t heightInPixels         = FreeImage_GetHeight(bitmap);

        // should be OK now
        diffuseTexture.mName          = assetPath.filename().string();
        diffuseTexture.mTextureWidth  = widthInPixels;
        diffuseTexture.mTextureHeight = heightInPixels;
        diffuseTexture.mTextureType   = ITexture::DIFFUSE;
        diffuseTexture.mTextureFormat = colorType == FIC_RGB ? ITexture::RGB : colorType == FIC_RGBALPHA ? ITexture::RGBA : ITexture::UNKNOWN_FORMAT;
        diffuseTexture.mTextureData.insert(diffuseTexture.mTextureData.begin(), FreeImage_GetBits(bitmap), FreeImage_GetBits(bitmap) + ((bitsPerPixel/8) * widthInPixels * heightInPixels));

        FreeImage_Unload(bitmap);
        
        return diffuseTexture;
    }



    /*
     * Attempts to get the asset type by parsing the asset name
     */
    AssetImporter::AssetType AssetImporter::GetAssetType(const char* assetName) const
    {
        // check if texture
        FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(assetName);
        if (imageFormat == FIF_UNKNOWN) 
            imageFormat = FreeImage_GetFIFFromFilename(assetName);

        if (imageFormat != FIF_UNKNOWN || FreeImage_FIFSupportsReading(imageFormat))
            return MATERIAL;
        else
            // default: try as model
            // TODO: check if loadable by assimp
            return MODEL;
            
    }

    /*
     * Creates default name for an asset
     */
    std::string AssetImporter::GetDefaultAssetName(AssetType assetType, uint32_t assetTypeNumber) const
    {
        std::stringstream ssName;

        switch (assetType)
        {
            case MODEL:     ssName << "model" << assetTypeNumber; break;
            case MATERIAL:  ssName << "material" << assetTypeNumber; break;
            default:        ssName << "unknown" << assetTypeNumber; break;
        }

        return ssName.str();
    }

    Mat4 AssetImporter::aiMat4ToJonsMat4(const aiMatrix4x4& aiMat) const
    {
        Mat4 jMat;

        jMat[0].x = aiMat.a1; jMat[0].y = aiMat.b1; jMat[0].z = aiMat.c1; jMat[0].w = aiMat.d1;
        jMat[1].x = aiMat.a2; jMat[1].y = aiMat.b2; jMat[1].z = aiMat.c2; jMat[1].w = aiMat.d2;
        jMat[2].x = aiMat.a3; jMat[2].y = aiMat.b3; jMat[2].z = aiMat.c3; jMat[2].w = aiMat.d3;
        jMat[3].x = aiMat.a4; jMat[3].y = aiMat.b4; jMat[3].z = aiMat.c4; jMat[3].w = aiMat.d4;

        return jMat;
    }

    Vec3 AssetImporter::aiColor3DToJonsVec3(const aiColor3D& color) const
    {
        Vec3 vec;

        vec.r = color.r;
        vec.g = color.g;
        vec.b = color.b;

        return vec;
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