#pragma once

#include "Types.h"
#include "IDMapTree.hpp"

#include <string>
#include <vector>
#include <functional>

namespace JonsEngine
{
	class Scene;
    class SceneNode;

    typedef std::function<void(SceneNode*)> OnSceneNodeDirtyFunc;
    typedef IDMapTree<SceneNode>::ItemID SceneNodeID;
    static const SceneNodeID INVALID_SCENE_NODE_ID = IDMapTree<SceneNode>::INVALID_ITEM_ID;

    class SceneNode
    {
    public:
        SceneNode(const std::string& nodeName, const SceneNodeID parent, const OnSceneNodeDirtyFunc& onDirty);
		SceneNode(const SceneNode& otherSceneNode);
        ~SceneNode();
	
        // TODO: for non-uniform scaling, need change in gbuffer shader
        void ScaleNode(const float scale);
        void TranslateNode(const Vec3& translateVec);
        void RotateNode(const float angle, const Vec3& rotateVec);

        void UpdateWorldMatrix();
        void UpdateWorldMatrix(const Mat4& parentMatrix);

		void SetVisible( bool visible ) { mVisibile = visible; }
		bool IsVisible() const { return mVisibile; }

        const Vec3& Position() const;
        const Vec3& Scale() const;
        const Mat4& GetWorldTransform() const;

        const std::string& GetName() const;
        SceneNodeID GetParentID() const;

	private:
		friend class Scene;
		void SetCallback( const OnSceneNodeDirtyFunc& onDirty );

    private:
        std::string mName;
        Mat4 mWorldTransform;
        Quaternion mOrientation;
        Vec3 mScale;
		Vec3 mTranslation;
		bool mVisibile = true;

        SceneNodeID mParent;
        OnSceneNodeDirtyFunc mOnDirtyFunc;
    };
}