#include "../pch.h"
#include "Input.h"
#include "../Window/AppWindow.h"
namespace IHCEngine::Core
{
	Input::Input()
		: keyActionArray{ InputAction::NONE },
		  mouseButtonActionArray{ InputAction::NONE },
	      anyKeyHeldCounter(0),
		  anyKeyDownCounter(0),
		  isKeyInputDirty(false),
		  isMouseButtonInputDirty(false)
	{
		Window::AppWindow::SetKeyCallback(std::bind(&Input::KeyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		Window::AppWindow::SetMouseButtonCallback(std::bind(&Input::MouseButtonCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		Window::AppWindow::SetMousePositionCallback(std::bind(&Input::MousePositionCallback, std::placeholders::_1, std::placeholders::_2));
		Window::AppWindow::SetMouseScrollCallback(std::bind(&Input::MouseScrollCallback, std::placeholders::_1, std::placeholders::_2));
	}

	void Input::Init()
	{
		GetInstance();
	}

	void Input::Update()
	{
		UpdateKeys();

		UpdateMouseButtons();

		ResetScrollDelta();
	}

	bool Input::IsKeyDown(int keyCode)
	{
		return instance->keyActionArray[keyCode] == InputAction::DOWN;
	}

	bool Input::IsKeyUp(int keyCode)
	{
		return instance->keyActionArray[keyCode] == InputAction::UP;
	}

	bool Input::IsKeyHeld(int keyCode)
	{
		return instance->keyActionArray[keyCode] == InputAction::HELD;
	}

	bool Input::IsMouseButtonDown(int mouseCode)
	{
		return instance->mouseButtonActionArray[mouseCode] == InputAction::DOWN;
	}

	bool Input::IsMouseButtonUp(int mouseCode)
	{
		return instance->mouseButtonActionArray[mouseCode] == InputAction::UP;
	}

	bool Input::IsMouseButtonHeld(int mouseCode)
	{
		return instance->mouseButtonActionArray[mouseCode] == InputAction::HELD;
	}

	bool Input::IsAnyKeyDown()
	{
		return instance->anyKeyDownCounter > 0;
	}

	bool Input::IsAnyKeyHeld()
	{
		return instance->anyKeyHeldCounter > 0;
	}



	void Input::UpdateKeys()
	{
		if (instance->isKeyInputDirty)
		{
			for (auto& keyIndex : instance->keyActionArray)
			{
				if (keyIndex == InputAction::DOWN)
				{
					keyIndex = InputAction::HELD;
					instance->anyKeyHeldCounter++;
					instance->anyKeyDownCounter--;
				}
				else if (keyIndex == InputAction::UP)
				{
					keyIndex = InputAction::NONE;
					instance->anyKeyHeldCounter--;
				}
			}
			instance->isKeyInputDirty = false;
		}
	}

	void Input::UpdateMouseButtons()
	{
		if (instance->isMouseButtonInputDirty)
		{
			for (auto& mouseButtonIndex : instance->mouseButtonActionArray)
			{
				if (mouseButtonIndex == InputAction::DOWN)
				{
					mouseButtonIndex = InputAction::HELD;
					instance->anyKeyHeldCounter++;
					instance->anyKeyDownCounter--;
				}
				else if (mouseButtonIndex == InputAction::UP)
				{
					mouseButtonIndex = InputAction::NONE;
					instance->anyKeyHeldCounter--;
				}
			}
			instance->isMouseButtonInputDirty = false;
		}
	}

	void Input::KeyCallback(int key, int scancode, int action, int mods)
	{
		const InputAction inputAction = GlfwInputActionToInputAction(action);
		if (inputAction != InputAction::REPEATED)
		{
			instance->keyActionArray[key] = inputAction;
			instance->isKeyInputDirty = true;
			if (inputAction == InputAction::DOWN)
			{
				instance->anyKeyDownCounter++;
			}
		}
	}

	void Input::MouseButtonCallback(int button, int action, int mods)
	{
		const InputAction inputAction = GlfwInputActionToInputAction(action);
		if (inputAction != InputAction::REPEATED)
		{
			instance->mouseButtonActionArray[button] = inputAction;
			instance->isMouseButtonInputDirty = true;
			if (inputAction == InputAction::UP)
			{
				instance->anyKeyDownCounter++;
			}
		}
	}

	void Input::MousePositionCallback(double xPosition, double yPosition)
	{
		instance->mousePosition.x = xPosition;
		instance->mousePosition.y = yPosition;
	}

	void Input::MouseScrollCallback(double xOffset, double yOffset)
	{
		instance->scrollDelta.x = xOffset;
		instance->scrollDelta.y = yOffset;
	}

	InputAction Input::GlfwInputActionToInputAction(int glfwKeyAction)
	{
		InputAction inputAction;

		switch (glfwKeyAction)
		{
		case GLFW_PRESS:
			inputAction = InputAction::DOWN;
			break;

		case GLFW_RELEASE:
			inputAction = InputAction::UP;
			break;

		// no GLFW_HOLD

		case GLFW_REPEAT:
			inputAction = InputAction::REPEATED;
			break;

		default:
			inputAction = InputAction::NONE;
			break;
		}

		return inputAction;
	}

	void Input::ResetScrollDelta()
	{
		instance->scrollDelta.x = 0;
		instance->scrollDelta.y = 0;
	}
}
