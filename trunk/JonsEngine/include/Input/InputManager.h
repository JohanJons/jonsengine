#pragma once

#include "include/Input/KeyCallback.h"
#include "include/Input/MouseCallback.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/EngineDefs.h"

#include <vector>

namespace JonsEngine
{
    struct MouseButtonEvent;
    struct MouseMotionEvent;
    struct KeyEvent;
    class IMemoryAllocator;
    class Logger;
    class InputBackend;


    /* InputManager definition */
    class InputManager
    {
    public:
        InputManager(const EngineSettings& engineSettings);
        ~InputManager();

        void RegisterMouseButtonCallback(const MouseButtonCallback& callback);
        void RegisterMouseMotionCallback(const MouseMotionCallback& callback);
        void RegisterKeyCallback(const KeyCallback& callback);

        template <typename T>
        void UnregisterMouseButtonCallback(const T& callback);
        template <typename T>
        void UnregisterMouseMotionCallback(const T& callback);
        template <typename T>
        void UnregisterKeyCallback(const T& callback);

        template <typename T>
        bool IsCallbackRegistered(const T& callback) const;
                                                        
        void ClearAllCallbacks();

        void ShowMouseCursor(bool show);
        void SetMousePosition(uint32_t x, uint32_t y);

        /*
         * Dispatches queued input events to all registered callbacks.
         * Is called by the Engine once per frame.
         */
        void Poll();


    private:
        void ClearAllEvents();
        InputBackend* CreateBackend(InputBackend::InputBackendType backend);

        void OnMouseButton(const MouseButtonEvent& ev);
        void OnMouseMotion(const MouseMotionEvent& ev);
        void OnKey(const KeyEvent& ev);

        Logger& mLogger;
        IMemoryAllocator& mMemoryAllocator;
        InputBackend* mInputBackend;

        std::vector<MouseButtonCallback> mMouseButtonCallbacks;
        std::vector<MouseMotionCallback> mMouseMotionCallbacks;
        std::vector<KeyCallback>         mKeyCallbacks;

        std::vector<MouseButtonEvent>    mMouseButtonEvents;
        std::vector<MouseMotionEvent>    mMouseMotionEvents;
        std::vector<KeyEvent>            mKeyEvents;
    };


    /* InputManager inlines */
    template <typename T>
    inline void InputManager::UnregisterMouseButtonCallback(const T& callback)
    {
        mMouseButtonCallbacks.erase(std::find(mMouseButtonCallbacks.begin(), mMouseButtonCallbacks.end(), callback));
    }
    
    template <typename T>
    inline void InputManager::UnregisterMouseMotionCallback(const T& callback)
    {
        mMouseMotionCallbacks.erase(std::find(mMouseMotionCallbacks.begin(), mMouseMotionCallbacks.end(), callback));
    }
    
    template <typename T>
    inline void InputManager::UnregisterKeyCallback(const T& callback)
    {
        mKeyCallbacks.erase(std::find(mKeyCallbacks.begin(), mKeyCallbacks.end(), callback));
    }

    template <typename T>
    inline bool InputManager::IsCallbackRegistered(const T& callback) const
    { 
        return ((std::find(mMouseButtonCallbacks.begin(), mMouseButtonCallbacks.end(), callback) != mMouseButtonCallbacks.end()) ||
                (std::find(mMouseMotionCallbacks.begin(), mMouseMotionCallbacks.end(), callback) != mMouseMotionCallbacks.end()) ||
                (std::find(mKeyCallbacks.begin(), mKeyCallbacks.end(), callback) != mKeyCallbacks.end()));
    }
    
}