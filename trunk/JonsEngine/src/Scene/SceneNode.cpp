#include "include/Scene/SceneNode.h"
#include "include/Core/Utils/Math.h"

#include "boost/foreach.hpp"
#include "boost/functional/hash.hpp"
#include "boost/lambda/lambda.hpp"
#include <algorithm>
#include "glm/gtx/quaternion.hpp"

namespace JonsEngine
{
    SceneNode::SceneNode(const std::string& nodeName) : mName(nodeName), mHashedID(boost::hash_value(nodeName)), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()),
                                                        mModelMatrix(1.0f), mOrientation(1.0f, 0.0f, 0.0f, 0.0f), mScale(1.0f, 1.0f, 1.0f), mTranslation(0.0f, 0.0f, 0.0f)
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
        std::vector<SceneNode*>::iterator iter = std::find_if(mChildNodes.begin(), mChildNodes.end(), *boost::lambda::_1 == nodeName);

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
        std::vector<SceneNode*>::iterator iter = std::find_if(mChildNodes.begin(), mChildNodes.end(), *boost::lambda::_1 == nodeName);

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
        std::vector<SceneNode*>::iterator iter = std::find_if(mChildNodes.begin(), mChildNodes.end(), boost::lambda::_1 == node);

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

        
    void SceneNode::Scale(const Vec3& scaleVec)                                
    { 
        mScale *= scaleVec;            
    }

    void SceneNode::Translate(const Vec3& translateVec)
    { 
        mTranslation += translateVec;
    }

    void SceneNode::Rotate(const float angle, const Vec3& rotateVec)       
    { 
        Quaternion rotation = AngleAxisToQuaternion(angle, rotateVec);
        mOrientation = mOrientation * rotation;
    }


    void SceneNode::UpdateModelMatrix(const Mat4& parentModelMatrix)
    {
        Mat4 modelMatrix(1.0f);

        modelMatrix = TranslateTransform(modelMatrix, mTranslation);
        modelMatrix *= QuaternionToMat4(mOrientation);
        modelMatrix = ScaleTransform(modelMatrix, mScale);

        mModelMatrix = parentModelMatrix * modelMatrix;

        BOOST_FOREACH(SceneNode* childNode, mChildNodes)
            childNode->UpdateModelMatrix(mModelMatrix);
    }


    bool SceneNode::operator==(const SceneNode& s1)
    {
        return mHashedID == s1.mHashedID;
    }
        
    bool SceneNode::operator==(const std::string& nodeName)
    {
        return mHashedID == boost::hash_value(nodeName);
    }
}
