#include "../../pch.h"
#include "AssetManager.h"

IHCEngine::Core::AssetManager::~AssetManager()
{
	textureRepository.ClearAssets();
	modelRepository.ClearAssets();
};