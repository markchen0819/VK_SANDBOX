#pragma once
#include "../pch.h"

namespace IHCEngine::Window
{
	class AppWindow
	{
	public:

		AppWindow(std::string windowName, unsigned int width, unsigned int height);
		~AppWindow();

		void Update();
		bool ShouldClose() { return glfwWindowShouldClose(window); };

		GLFWwindow* GetWindowHandle() { return window; };
		int GetHeight() { return height; };
		int GetWidth() { return width; };
		void SetWidthHeight(unsigned int w, unsigned int h) { width = w; height = h; }
		bool IsWindowResized() { return windowResized; };
		void SetWindowResized(bool b) { windowResized = b; }
		std::string GetName() { return  windowName; }

		// Callbacks
		static void SetKeyCallback(std::function<void(int, int, int, int)> keyCallback);
		static void SetMouseButtonCallback(std::function<void(int, int, int)> mouseButtonCallback);
		static void SetMousePositionCallback(std::function<void(double, double)> mousePositionCallback);
		static void SetMouseScrollCallback(std::function<void(double, double)> mouseScrollCallback);

	private:

		GLFWwindow* window;
		std::string windowName;
		unsigned int height;
		unsigned int width;
		bool windowResized = false;

		// Callbacks
		static inline std::function<void(int, int, int, int)> keyCallback = nullptr;
		static inline std::function<void(int, int, int)> mouseButtonCallback = nullptr;
		static inline std::function<void(double, double)> mousePositionCallback = nullptr;
		static inline std::function<void(double, double)> mouseScrollCallback = nullptr;
		// GLFW Callbacks
		static void GLFW_WindowResizeCallback(GLFWwindow* window, int width, int height);
		static void GLFW_MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void GLFW_MousePositionCallback(GLFWwindow* window, double xPosition, double yPosition);

	};
}


