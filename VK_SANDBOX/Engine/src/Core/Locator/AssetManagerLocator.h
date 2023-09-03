#pragma once

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
