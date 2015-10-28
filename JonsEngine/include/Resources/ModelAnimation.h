#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"

#include <string>
#include <vector>
#include <utility>
#include <limits>

namespace JonsEngine
{
    class ModelAnimation
    {
    public:
        ModelAnimation(const PackageAnimation& pkgAnimation);
        ~ModelAnimation();
        
        const Mat4& GetNodeTransform(const ModelNodeIndex nodeIndex, const Milliseconds elapsedTime) const;
        const std::string& GetName() const;
        Milliseconds GetAnimationDuration() const;


    private:
        typedef std::pair<Mat4, double> NodeTransformTimestamp;
        typedef std::vector<NodeTransformTimestamp> NodeTransformsContainer;
        typedef std::pair<ModelNodeIndex, NodeTransformsContainer::size_type> NodeIDMap;
    

        std::string mName;
        Milliseconds mAnimationDuration;

        std::vector<NodeIDMap> mNodeIDMapping;
        NodeTransformsContainer mNodeTransforms;
    };
}