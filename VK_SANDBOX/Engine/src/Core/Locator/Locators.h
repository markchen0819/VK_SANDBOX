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


#include "../../Graphics/GraphicsManager.h"
namespace IHCEngine::Core
{
	class GraphicsManagerLocator
	{
	public:
		static IHCEngine::Graphics::GraphicsManager* GetGraphicsManager()
		{
			return systemInstance;
		}

		static void Provide(IHCEngine::Graphics::GraphicsManager* system)
		{
			systemInstance = system;
		}

	private:
		inline static IHCEngine::Graphics::GraphicsManager* systemInstance{ nullptr };
	};
}


#include "../Asset/AssetManager.h"
namespace IHCEngine::Core
{
	class AssetManagerLocator
	{
	public:
		static AssetManager* GetAssetManager()
		{
			return systemInstance;
		}

		static void Provide(AssetManager* system)
		{
			systemInstance = system;
		}

	private:
		inline static AssetManager* systemInstance{ nullptr };
	};
}


#include "../Scene/SceneManager.h"
namespace IHCEngine::Core
{
	class SceneManagerLocator
	{
	public:
		static SceneManager* GetSceneManager()
		{
			return systemInstance;
		}

		static void Provide(SceneManager* system)
		{
			systemInstance = system;
		}

	private:
		inline static SceneManager* systemInstance{ nullptr };
	};
}