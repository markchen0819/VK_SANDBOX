#pragma once

#include "../../Graphics/GraphicsManager.h"
#include "../../Graphics/GraphicsAssetCreator.h"

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

