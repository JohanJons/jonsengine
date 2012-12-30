#include "include/Scene/SceneNode.h"
#include "include/Core/Utils/Math.h"

#include "boost/foreach.hpp"
#include "boost/functional/hash.hpp"
#include "boost/lambda/lambda.hpp"
#include <algorithm>

namespace JonsEngine
{
    SceneNode::SceneNode(const std::string& nodeName) : mName(nodeName), mHashedID(boost::hash_value(nodeName)), mTransform(1.0f), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator())
    {
    }
        
    SceneNode::~SceneNode()
    {
        BOOST_FOREACH(SceneNode* childNode, mChildNodes)
            mMemoryAllocator.DeallocateObject(childNode);
    }


    SceneNode* SceneNode::CreateChildNode(const std::string& nodeName)
    {
        mChildNodes.push_back(mMemoryAllocator.AllocateObject<SceneNode>(nodeName));

        return mChildNodes.back();
    }
        
    SceneNode* SceneNode::FindChildNode(const std::string& nodeName)
    {
        SceneNode* ret = NULL;
        vector<SceneNode*>::iterator iter = std::find_if(mChildNodes.begin(), mChildNodes.end(), *boost::lambda::_1 == nodeName);

        if (iter == mChildNodes.end())
        {
            BOOST_FOREACH(SceneNode* node, mChildNodes)
            {
                ret = node->FindChildNode(nodeName);
                if (ret && *ret == nodeName)
                    break;
            }
        }
        else
            ret = *iter;

        return ret;
    }
        
    bool SceneNode::DeleteChildNode(const std::string& nodeName)
    {
        bool isFound = false;
        vector<SceneNode*>::iterator iter = std::find_if(mChildNodes.begin(), mChildNodes.end(), *boost::lambda::_1 == nodeName);

        if (iter != mChildNodes.end())
        {
            mMemoryAllocator.DeallocateObject(*iter);
            mChildNodes.erase(iter);
            isFound = true;
        }
        else
            BOOST_FOREACH(SceneNode* childNode, mChildNodes)
            {
                isFound = childNode->DeleteChildNode(nodeName);
                if (isFound)
                    break;
            }

        return isFound;
    }
        
    bool SceneNode::DeleteChildNode(SceneNode* node)
    {
        bool isFound = false;
        vector<SceneNode*>::iterator iter = std::find_if(mChildNodes.begin(), mChildNodes.end(), boost::lambda::_1 == node);

        if (iter != mChildNodes.end())
        {
            mMemoryAllocator.DeallocateObject(*iter);
            mChildNodes.erase(iter);
            isFound = true;
        }
        else
            BOOST_FOREACH(SceneNode* childNode, mChildNodes)
            {
                isFound = childNode->DeleteChildNode(node);
                if (isFound)
                    break;
            }

        return isFound;
    }

        
    void SceneNode::Scale(Vec3 scaleVec)
    {
        mTransform = ScaleTransform(mTransform, scaleVec);
    }

    void SceneNode::Translate(Vec3 translateVec)
    {
        mTransform = TranslateTransform(mTransform, translateVec);
    }
        
    void SceneNode::Rotate(Vec3 rotateVec, const float angle)
    {
        mTransform = RotateTransform(mTransform, angle, rotateVec);
    }


    void SceneNode::SetMesh()                                       { mNodeMesh.reset();    }
    void SceneNode::SetMesh(MeshPtr mesh)                           { mNodeMesh = mesh;     }
    MeshPtr SceneNode::GetMesh()                                    { return mNodeMesh;     }


    const std::string& SceneNode::GetNodeName() const               { return mName;         }
    const vector<SceneNode*>& SceneNode::GetChildNodes() const      { return mChildNodes;   }


    bool SceneNode::operator==(const SceneNode& s1)
    {
        return mHashedID == s1.mHashedID;
    }
        
    bool SceneNode::operator==(const std::string& nodeName)
    {
        return mHashedID == boost::hash_value(nodeName);
    }
}
