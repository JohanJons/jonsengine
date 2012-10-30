#pragma once

#include "include/Input/InputManager.h"

#include "include/Input/GLFW/GLFWInputBackend.h"
#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"


namespace JonsEngine
{
    InputManager::InputManager(const EngineSettings& engineSettings) : mLogger(Globals::GetInputLogger()), mMemoryAllocator(Globals::GetDefaultHeapAllocator()), mEngineSettings(engineSettings), mInputBackend(CreateBackend(engineSettings.InputBackend))
    {
    }
        
    InputManager::~InputManager()
    {
        ClearAllCallbacks();
        ClearAllEvents();

        mMemoryAllocator.DeallocateObject(mInputBackend);
    }

    void InputManager::RegisterMouseButtonCallback(const MouseButtonCallback& callback)                { mMouseButtonCallbacks.push_back(callback);  }
    void InputManager::RegisterMouseMotionCallback(const MouseMotionCallback& callback)                { mMouseMotionCallbacks.push_back(callback);  }  
    void InputManager::RegisterKeyCallback(const KeyCallback& callback)                                { mKeyCallbacks.push_back(callback);          }

    void InputManager::ClearAllCallbacks()
    {
        mMouseButtonCallbacks.clear();
        mMouseMotionCallbacks.clear();
        mKeyCallbacks.clear();
    }

    void InputManager::Poll()
    {
        // Dispatch waiting events for each event type to registered callbacks
        BOOST_FOREACH(MouseButtonEvent ev, mMouseButtonEvents)
        {
            BOOST_FOREACH(MouseButtonCallback callback, mMouseButtonCallbacks)
                callback(ev);
        }

        BOOST_FOREACH(MouseMotionEvent ev, mMouseMotionEvents)
        {
            BOOST_FOREACH(MouseMotionCallback callback, mMouseMotionCallbacks)
                callback(ev);
        }

        BOOST_FOREACH(KeyEvent ev, mKeyEvents)
        {
            BOOST_FOREACH(KeyCallback callback, mKeyCallbacks)
                callback(ev);
        }
    }

    void InputManager::ClearAllEvents()
    {
        mMouseButtonEvents.clear();
        mMouseMotionEvents.clear();
        mKeyEvents.clear();
    }

    InputBackend* InputManager::CreateBackend(InputBackend::InputBackendType backend)
    {
        if (backend == InputBackend::GLFW)
            return mMemoryAllocator.AllocateObject<GLFWInputBackend>(boost::bind(&InputManager::OnMouseButton, this, _1), 
                                                                     boost::bind(&InputManager::OnMouseMotion, this, _1),
                                                                     boost::bind(&InputManager::OnKey, this, _1));
        else
            return NULL;
    }

    void InputManager::OnMouseButton(const MouseButtonEvent& ev)
    {
        mMouseButtonEvents.push_back(ev);
    }

    void InputManager::OnMouseMotion(const MouseMotionEvent& ev)
    {
        mMouseMotionEvents.push_back(ev);
    }

    void InputManager::OnKey(const KeyEvent& ev)
    {
        mKeyEvents.push_back(ev);
    }
}