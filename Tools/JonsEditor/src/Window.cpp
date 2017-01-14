#include "include/Window.h"

#include "include/Window/WindowSettings.h"

#include <functional>

using namespace JonsEngine;

namespace JonsEditor
{
	static const WindowSettings gDefaultWindowSettings = {"JonsEditor", 1920, 1080, false, 0};
	constexpr float gDefaultCameraMovespeed = 0.1f;


	Window::Window() :
		mWindowManager(gDefaultWindowSettings, mHeapAllocator, mLogger),
		mCameraMovespeed(gDefaultCameraMovespeed)
	{
		mWindowManager.ShowMouseCursor(true);
		SetupInputCallbacks();
	}

	void Window::Poll()
	{
		mWindowManager.Poll();
	}


	void Window::SetupInputCallbacks()
	{
		using namespace std::placeholders;

		mWindowManager.SetMousePositionCallback(std::bind(&Window::OnMousePositionEvent, this, _1));
		mWindowManager.SetKeyCallback(std::bind(&Window::OnKeyEvent, this, _1));
	}


	void Window::OnKeyEvent(const KeyEvent& evnt)
	{
		if (evnt.mState == KeyEvent::KeyState::STATE_PRESSED || evnt.mState == KeyEvent::KeyState::STATE_REPEAT)
		{
			switch (evnt.mKey)
			{
				case Key::A: mCamera.TranslateCamera(-mCamera.Right() * mCameraMovespeed);   break;
				case Key::W: mCamera.TranslateCamera(mCamera.Forward() * mCameraMovespeed);  break;
				case Key::S: mCamera.TranslateCamera(-mCamera.Forward() * mCameraMovespeed); break;
				case Key::D: mCamera.TranslateCamera(mCamera.Right() * mCameraMovespeed);    break;
				default: break;
			}
		}
	}

	void Window::OnMousePositionEvent(const MousePositionEvent& evnt)
	{
		const float sens = 0.1f;
		float newXPos = (float)evnt.mRelativePosX * sens;
		float newYPos = (float)evnt.mRelativePosY * sens;

		mCamera.RotateCamera(newXPos, newYPos);
	}
}