#include "include/Editor.h"

using namespace JonsEngine;

namespace JonsEditor
{
	EngineSettings GetDefaultEngineSettings();

	constexpr float gDefaultCameraMovespeed = 0.1f;


	Editor::Editor() :
		mEngine(GetDefaultEngineSettings()),
		mIsRunning(false),
		mCameraMovespeed(gDefaultCameraMovespeed)
	{
		mEngine.GetWindow().ShowMouseCursor(true);
		SetupInputCallbacks();
		SetupGeometry();
	}


	void Editor::Run()
	{
		const JonsEngine::DebugOptions debugOpts;

		for (mIsRunning = true; mIsRunning ;)
		{
			mEngine.Tick(debugOpts);
		}
	}


	void Editor::SetupInputCallbacks()
	{
		using namespace std::placeholders;

		mEngine.GetWindow().SetKeyCallback(std::bind(&Editor::OnKeyEvent, this, _1));
		mEngine.GetWindow().SetMouseButtonCallback(std::bind(&Editor::OnMouseButtonEvent, this, _1));
		mEngine.GetWindow().SetMouseMovementCallback(std::bind(&Editor::OnMouseMovementEvent, this, _1));
	}

	void Editor::SetupGeometry()
	{
		auto modelId = mEngine.GetResourceManifest().CreateRectangle("Map", 5, 1, 5);

		auto& activeScene = mEngine.GetSceneManager().GetActiveScene();
		auto nodeId = activeScene.GetRootNodeID();
		auto actorId = activeScene.CreateStaticActor("ActorMap", modelId, nodeId);

		assert(modelId && nodeId && actorId);
		
		activeScene.GetRootNode().RotateNode(90.0f, Vec3(0.0f, 1.0f, 0.0f));
		activeScene.SetAmbientLight(Vec4(1.0f));

		auto& camera = activeScene.GetSceneCamera();
		camera.TranslateCamera(Vec3(0.0f, 3.0f, 2.0f));
		camera.RotateCamera(0.0f, 45.0f);
	}


	void Editor::OnKeyEvent(const KeyEvent& evnt)
	{
		if (evnt.mState == KeyEvent::KeyState::STATE_PRESSED || evnt.mState == KeyEvent::KeyState::STATE_REPEAT)
		{
			auto& camera = mEngine.GetSceneManager().GetActiveScene().GetSceneCamera();

			switch (evnt.mKey)
			{
			case Key::A: camera.TranslateCamera(-camera.Right() * mCameraMovespeed);   break;
			case Key::W: camera.TranslateCamera(camera.Forward() * mCameraMovespeed);  break;
			case Key::S: camera.TranslateCamera(-camera.Forward() * mCameraMovespeed); break;
			case Key::D: camera.TranslateCamera(camera.Right() * mCameraMovespeed);    break;
			case Key::ESC: mIsRunning = false; break;
			default: break;
			}
		}
	}

	void Editor::OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt)
	{
		auto& window = mEngine.GetWindow();
		auto& renderer = mEngine.GetRenderer();
		auto windowDimensions = window.GetWindowDimensions();
		auto mousePosition = window.GetCurrentMousePosition();

		auto& camera = mEngine.GetSceneManager().GetActiveScene().GetSceneCamera();
		float cameraFOV = camera.GetFOV();
		Mat4 viewTransform = camera.GetCameraTransform();
		Mat4 projTransform = PerspectiveMatrixFov(cameraFOV, windowDimensions.x / static_cast<float>(windowDimensions.y), renderer.GetZNear(), renderer.GetZFar());
		Mat4 invViewProj = glm::inverse(projTransform * viewTransform);

		float x = 2.0f * mousePosition.x / windowDimensions.x - 1;
		float y = -2.0f * mousePosition.y / windowDimensions.y + 1;
		float z = renderer.GetDepthValue(mousePosition);
		Vec4 point4{x, y, 0, 1};
		point4 = invViewProj * point4;
		Vec3 point3(point4);
		//Point3D point3D = new Point3D(x, y, 0);
		//return viewProjectionInverse.multiply(point3D);

		_RPT1(0, "%f x %f x %f\n", point3.x, point3.y, point3.z);
	}

	void Editor::OnMouseMovementEvent(const MouseMovementEvent& evnt)
	{
		const float sens = 0.1f;
		float newXPos = (float)evnt.x * sens;
		float newYPos = (float)evnt.y * sens;

		auto& camera = mEngine.GetSceneManager().GetActiveScene().GetSceneCamera();
		camera.RotateCamera(newXPos, newYPos);
	}


	EngineSettings GetDefaultEngineSettings()
	{
		EngineSettings ret;

		ret.mWindowTitle = "JonsEditor";

		return ret;
	}
}