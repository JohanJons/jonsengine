#pragma once

#include "include/Core/Containers/Vector.h"

#include "GL/glfw.h"

namespace JonsEngine
{
	class IKeyListener;
	class IMouseListener;
	class EngineSettings;

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		bool Init(const EngineSettings& engineSettings);
		void Destroy();

		void AddKeyListener(IKeyListener* listener);
		void RemoveKeyListener(IKeyListener* listener);
		void ClearKeyListeners();
		const Vector<IKeyListener*>& GetKeyListeners() const;

		void AddMouseListener(IMouseListener* listener);
		void RemoveMouseListener(IMouseListener* listener);
		void ClearMouseListeners();
		const Vector<IMouseListener*>& GetMouseListeners() const;

	private:
		void GLFWCALL glfwCharCallback(int, int);

		Vector<IKeyListener*> mKeyListeners;
		Vector<IMouseListener*> mMouseListeners;
	};
}