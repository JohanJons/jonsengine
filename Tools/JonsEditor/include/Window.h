#pragma once

#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Logging/Logger.h"
#include "include/Window/WindowManager.h"
#include "include/Scene/Camera.h"

namespace JonsEditor
{
	class Window
	{
	public:
		Window();

		void Poll();


	private:
		void SetupInputCallbacks();

		void OnMousePositionEvent(const JonsEngine::MousePositionEvent& evnt);
		void OnKeyEvent(const JonsEngine::KeyEvent& evnt);


		JonsEngine::HeapAllocator mHeapAllocator;
		JonsEngine::Logger mLogger;
		JonsEngine::WindowManager mWindowManager;
		JonsEngine::Camera mCamera;

		float mCameraMovespeed;
	};
}