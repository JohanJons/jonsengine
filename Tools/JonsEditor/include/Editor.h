#pragma once

//#include "include/Window.h"
#include "include/Core/Engine.h"

namespace JonsEditor
{
	class Editor
	{
	public:
		Editor();

		void Run();


	private:
		void SetupInputCallbacks();
		void SetupGeometry();

		void OnKeyEvent(const JonsEngine::KeyEvent& evnt);
		void OnMousePositionEvent(const JonsEngine::MousePositionEvent& evnt);
		void OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt);


		JonsEngine::Engine mEngine;

		bool mIsRunning;
		float mCameraMovespeed;
	};
}