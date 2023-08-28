#pragma once

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