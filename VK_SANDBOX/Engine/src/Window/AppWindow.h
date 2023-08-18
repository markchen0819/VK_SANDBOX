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

	private:

		GLFWwindow* window;
		std::string windowName;
		unsigned int height;
		unsigned int width;
		bool windowResized = false;

	};
}


