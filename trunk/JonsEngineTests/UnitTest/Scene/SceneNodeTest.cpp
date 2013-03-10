#include "SceneNodeTest.h"

#include "include/Scene/Scene.h"
#include "include/Scene/SceneNode.h"

#include <string>

namespace JonsEngine
{
    /**
	 * CreateChildNode test
	 */
	TEST_F(SceneNodeTest, CreateChildNode)
	{
        const std::string sceneName = "TestScene";
        const std::string nodeName1 = "TestNode1";
        const std::string nodeName2 = "TestNode2";
        const std::string nodeName3 = "TestNode3";

        Scene* scene = mEngine->GetSceneManager().CreateScene(sceneName);
        
        SceneNodePtr rootChild = scene->GetRootNode().CreateChildNode(nodeName1);
        scene->GetRootNode().CreateChildNode(nodeName2);
        rootChild->CreateChildNode(nodeName3);

        ASSERT_EQ(scene->GetRootNode().GetChildNodes().size(), 2);
        ASSERT_EQ(rootChild->GetChildNodes().size(), 1);

        mEngine->GetSceneManager().DeleteScene(sceneName);
	}


    /**
	 * FindChildNode test
	 */
	TEST_F(SceneNodeTest, FindChildNode)
	{
        const std::string sceneName = "TestScene";
        const std::string nodeName1 = "TestNode1";
        const std::string nodeName2 = "TestNode2";
        const std::string nodeName3 = "TestNode3";
        const std::string nodeName4 = "NotCreated";

        Scene* scene = mEngine->GetSceneManager().CreateScene(sceneName);
        
        SceneNodePtr rootChild = scene->GetRootNode().CreateChildNode(nodeName1);
        scene->GetRootNode().CreateChildNode(nodeName2);
        rootChild->CreateChildNode(nodeName3);

        ASSERT_EQ(scene->GetRootNode().FindChildNode(nodeName3)->GetNodeName(), nodeName3);
        ASSERT_EQ(rootChild->FindChildNode(nodeName3)->GetNodeName(), nodeName3);
        ASSERT_EQ(scene->GetRootNode().FindChildNode(nodeName2)->GetNodeName(), nodeName2);

        mEngine->GetSceneManager().DeleteScene(sceneName);
	}


    /**
	 * DeleteChildNode test
	 */
	TEST_F(SceneNodeTest, DeleteChildNode)
	{
        const std::string sceneName = "TestScene";
        const std::string nodeName1 = "TestNode1";
        const std::string nodeName2 = "TestNode2";
        const std::string nodeName3 = "TestNode3";
        const std::string nodeName4 = "NotCreated";

        Scene* scene = mEngine->GetSceneManager().CreateScene(sceneName);
        
        SceneNodePtr rootChild = scene->GetRootNode().CreateChildNode(nodeName1);
        scene->GetRootNode().CreateChildNode(nodeName2);
        rootChild->CreateChildNode(nodeName3);

        ASSERT_EQ(scene->GetRootNode().FindChildNode(nodeName3)->GetNodeName(), nodeName3);
        ASSERT_EQ(rootChild->FindChildNode(nodeName3)->GetNodeName(), nodeName3);
        ASSERT_EQ(scene->GetRootNode().FindChildNode(nodeName2)->GetNodeName(), nodeName2);

        ASSERT_EQ(scene->GetRootNode().RemoveChildNode(scene->GetRootNode().FindChildNode(nodeName3)), true);

        rootChild->CreateChildNode(nodeName3);
        ASSERT_EQ(scene->GetRootNode().RemoveChildNode(rootChild), true);

        ASSERT_EQ(scene->GetRootNode().RemoveChildNode(nodeName4), false);

        mEngine->GetSceneManager().DeleteScene(sceneName);
	}
}