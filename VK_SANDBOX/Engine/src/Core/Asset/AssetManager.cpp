#include "../../pch.h"
#include "AssetManager.h"

IHCEngine::Core::AssetManager::~AssetManager()
{
	textureRepository.ClearAssets();
	meshRepository.ClearAssets();
	modelRepository.ClearAssets();
};
