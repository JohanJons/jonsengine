#pragma once

#include "include/Input/InputManager.h"

#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"
#include "GL/glfw.h"

#include <algorithm>


namespace JonsEngine
{
	static InputManager* gInputManagerInstance = NULL;

	InputManager::InputManager() : mLogger(Globals::GetInputLogger())
	{
	}
		
	InputManager::~InputManager()
	{
	}

	bool InputManager::Init(const EngineSettings& engineSettings)
	{
		glfwSetCharCallback(glfwCharCallback);
		gInputManagerInstance = this;

		GLenum glfwErr = glfwInit();
		if (glfwErr != GL_TRUE)
		{
			JONS_LOG_ERROR(mLogger, "Engine::Init(): Unable to initialize GLFW!")
			return false;
		}

		return true;
	}
		
	void InputManager::Destroy()
	{
	}


	void InputManager::AddKeyListener(IKeyListener* listener)
	{
		mKeyListeners.push_back(listener);
	}
		
	void InputManager::RemoveKeyListener(IKeyListener* listener)
	{
		mKeyListeners.erase(std::find(mKeyListeners.begin(), mKeyListeners.end(), listener));
	}
		
	void InputManager::ClearKeyListeners()
	{
		mKeyListeners.clear();
	}
		
	const Vector<IKeyListener*>& InputManager::GetKeyListeners() const
	{
		return mKeyListeners;
	}

		
	void InputManager::AddMouseListener(IMouseListener* listener)
	{
		mMouseListeners.push_back(listener);
	}
		
	void InputManager::RemoveMouseListener(IMouseListener* listener)
	{
		mMouseListeners.erase(std::find(mMouseListeners.begin(), mMouseListeners.end(), listener));
	}
		
	void InputManager::ClearMouseListeners()
	{
		mMouseListeners.clear();
	}
		
	const Vector<IMouseListener*>& InputManager::GetMouseListeners() const
	{
		return mMouseListeners;
	}


	void GLFWCALL InputManager::glfwCharCallback(int, int)
	{

	}
}