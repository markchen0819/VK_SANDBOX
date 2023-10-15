#pragma once

namespace IHCEngine::Core
{
	enum class InputAction
	{
		NONE,
		DOWN,
		UP,
		HELD,
		REPEATED,
	};

	class Input
	{
	public:
		// no duplication
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		static void Init();
		static void Update();

		static bool IsKeyDown(int keyCode);
		static bool IsKeyUp(int keyCode);
		static bool IsKeyHeld(int keyCode);

		static bool IsMouseButtonDown(int mouseCode);
		static bool IsMouseButtonUp(int mouseCode);
		static bool IsMouseButtonHeld(int mouseCode);

		static const glm::vec2 GetScrollDelta() { return instance->scrollDelta; }
		static const glm::vec2 GetMousePosition() { return instance->mousePosition; }

		static bool IsAnyKeyDown();
		static bool IsAnyKeyHeld();

	private:
		Input();
		static Input* GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<Input>(new Input()); // Static local variable
			}
			return instance.get();
		}
		static inline std::unique_ptr<Input> instance = nullptr;

		static constexpr int MAX_NUMBER_OF_KEYS = 350;
		static constexpr int MAX_NUMBER_OF_MOUSEBUTTONS = 16;

		std::array<InputAction, static_cast<int>(MAX_NUMBER_OF_KEYS)> keyActionArray;
		std::array<InputAction, static_cast<int>(MAX_NUMBER_OF_MOUSEBUTTONS)> mouseButtonActionArray;

		unsigned int anyKeyHeldCounter;
		unsigned int anyKeyDownCounter;

		bool isKeyInputDirty;
		bool isMouseButtonInputDirty;

		glm::vec2 mousePosition;
		glm::vec2 scrollDelta;

		static void UpdateKeys();
		static void UpdateMouseButtons();
		static void ResetScrollDelta();

		// callbacks
		static void KeyCallback(int key, int scancode, int action, int mods);
		static void MouseButtonCallback(int button, int action, int mods);
		static void MousePositionCallback(double xPosition, double yPosition);
		static void MouseScrollCallback(double xOffset, double yOffset);

		// conversion
		static InputAction GlfwInputActionToInputAction(int glfwKeyAction);
	};
}
