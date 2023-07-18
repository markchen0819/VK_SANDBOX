#pragma once
#include "../pch.h"

namespace IHCEngine::Window
{
	class AppWindow
	{
	public:

		AppWindow(const std::string& windowName, const unsigned int width, const unsigned int height);
		~AppWindow() = default;

		void Update();
		void Shutdown();
		bool ShouldClose() { return glfwWindowShouldClose(window); };

		GLFWwindow* GetWindowHandle() { return window; };
		int GetHeight() { return height; };
		int GetWidth() { return width; };
		bool IsWindowResized() { return windowResized; };
		void SetWindowResized(bool b) { windowResized = b; }

	private:

		GLFWwindow* window;
		std::string windowName;
		unsigned int height;
		unsigned int width;
		bool windowResized = false;

	};
}


