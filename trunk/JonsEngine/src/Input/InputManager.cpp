#pragma once

#include "include/Input/InputManager.h"

#include "boost/bind.hpp"

#include <algorithm>


namespace JonsEngine
{
	InputManager::InputManager()
	{
	}
		
	InputManager::~InputManager()
	{
	}

	bool InputManager::Init(const EngineSettings& engineSettings)
	{
		glfwSetCharCallback(boost::bind(&InputManager::glfwCharCallback, this, _1, _1));

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