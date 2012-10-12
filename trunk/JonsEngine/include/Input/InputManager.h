#pragma once

#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
	class IKeyListener;
	class IMouseListener;
	class Logger;
	struct EngineSettings;

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
		static void glfwCharCallback(int, int);

		Logger& mLogger;
		Vector<IKeyListener*> mKeyListeners;
		Vector<IMouseListener*> mMouseListeners;
	};
}