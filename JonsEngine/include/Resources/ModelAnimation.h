#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
//#include "include/Core/Containers/IDMap.hpp"

#include <string>
#include <vector>
#include <utility>

namespace JonsEngine
{
    class ModelAnimation
    {
    public:
        ModelAnimation(const PackageAnimation& pkgAnimation);
        ~ModelAnimation();
        
        const Mat4& GetNodeTransform(const ModelNode::NodeID nodeID, const double timestampInSeconds);

        double GetTotalDurationInSeconds() const;
        const std::string& GetName() const;


    private:
        typedef std::pair<Mat4, double> NodeTransformTimestamp;
        typedef std::vector<NodeTransformTimestamp> NodeTransformsContainer;
        typedef std::pair<ModelNode::NodeID, NodeTransformsContainer::size_type> NodeIDMap;
    
        std::string mName;
        double mDurationInSeconds;
        bool mIsPlaying;

        std::vector<NodeIDMap> mNodeIDMapping;
        NodeTransformsContainer mNodeTransforms;
    };
}