#pragma once

#include "include/Input/InputManager.h"

#include "include/Input/GLFW/GLFWInputBackend.h"
#include "include/Input/DummyInputBackend.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"


namespace JonsEngine
{
    InputManager::InputManager(const EngineSettings& engineSettings) : mLogger(Logger::GetInputLogger()), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mInputBackend(CreateBackend(engineSettings.InputBackend))
    {
    }
        
    InputManager::~InputManager()
    {
        ClearAllCallbacks();
        ClearAllEvents();

        mMemoryAllocator.DeallocateObject(mInputBackend);
    }

    void InputManager::RegisterMouseButtonCallback(const MouseButtonCallback& callback)
    { 
        mMouseButtonCallbacks.push_back(callback);

        if (!mInputBackend->IsMouseButtonCallbackSet())
            mInputBackend->SetMouseButtonCallback(boost::bind(&InputManager::OnMouseButton, this, _1));
    }

    void InputManager::RegisterMouseMotionCallback(const MouseMotionCallback& callback)
    { 
        mMouseMotionCallbacks.push_back(callback);

        if (!mInputBackend->IsMouseMotionCallbackSet())
            mInputBackend->SetMouseMotionCallback(boost::bind(&InputManager::OnMouseMotion, this, _1));
    }  

    void InputManager::RegisterKeyCallback(const KeyCallback& callback)
    { 
        mKeyCallbacks.push_back(callback);

        if (!mInputBackend->IsKeyCallbackSet())
            mInputBackend->SetKeyCallback(boost::bind(&InputManager::OnKey, this, _1));
    }

    void InputManager::ClearAllCallbacks()
    {
        mMouseButtonCallbacks.clear();
        mMouseMotionCallbacks.clear();
        mKeyCallbacks.clear();
    }


    void InputManager::ShowMouseCursor(bool show)
    {
        mInputBackend->ShowMouseCursor(show);
    }
        
    void InputManager::SetMousePosition(uint32_t x, uint32_t y)
    {
        mInputBackend->SetMousePosition(x, y);
    }


    void InputManager::Poll()
    {
        // Dispatch waiting events for each event type to registered callbacks
        BOOST_FOREACH(MouseButtonEvent& ev, mMouseButtonEvents)
        {
            BOOST_FOREACH(MouseButtonCallback& callback, mMouseButtonCallbacks)
                callback(ev);
        }

        BOOST_FOREACH(MouseMotionEvent& ev, mMouseMotionEvents)
        {
            BOOST_FOREACH(MouseMotionCallback& callback, mMouseMotionCallbacks)
                callback(ev);
        }

        BOOST_FOREACH(KeyEvent& ev, mKeyEvents)
        {
            BOOST_FOREACH(KeyCallback& callback, mKeyCallbacks)
                callback(ev);
        }

        ClearAllEvents();
    }

    void InputManager::ClearAllEvents()
    {
        mMouseButtonEvents.clear();
        mMouseMotionEvents.clear();
        mKeyEvents.clear();
    }

    InputBackend* InputManager::CreateBackend(InputBackend::InputBackendType backend)
    {
        switch (backend)
        {
            case InputBackend::GLFW:
                return mMemoryAllocator.AllocateObject<GLFWInputBackend>();
                break;


            default:
                return mMemoryAllocator.AllocateObject<DummyInputBackend>();
                break;

        }
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