#pragma once

#include "../../Engine/src/Window/AppWindow.h"
namespace IHCEngine::Core
{
	class AppWindowLocator
	{
	public:
		static IHCEngine::Window::AppWindow* GetAppWindow()
		{
			return systemInstance;
		}

		static void Provide(IHCEngine::Window::AppWindow* system)
		{
			systemInstance = system;
		}

	private:
		inline static IHCEngine::Window::AppWindow* systemInstance{ nullptr };
	};
}
