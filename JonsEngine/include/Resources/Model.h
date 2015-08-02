#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Resources/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
//#include "include/Core/Containers/DataPool.hpp"
//#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class DX11Renderer;

    class Model
    {
    public:
        Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);
        Model(const PackageModel& pkgModel);
        ~Model();

        ModelNode& GetRootNode();


        const std::string mName;


    private:
        typedef std::vector<ModelNode> NodeContainer;
        typedef std::vector<Mesh> MeshContainer;

        NodeContainer mNodes;
        MeshContainer mMeshes;
        //ModelNode mRootNode;
        // TODO
        //DataPool<Mat4> mTransformCache;
        //IDMap<ModelNode> mModelNodes;
        //IDMap<Mesh> mMeshes;
    };
}