#include "../../pch.h"
#include "AssetManager.h"

IHCEngine::Core::AssetManager::~AssetManager()
{
	textureRepository.ClearAssets();
	meshRepository.ClearAssets();
	modelRepository.ClearAssets();
	animationRepository.ClearAssets();
}

void IHCEngine::Core::AssetManager::ClearAllAssetRepositories()
{
	textureRepository.ClearAssets();
	meshRepository.ClearAssets();
	modelRepository.ClearAssets();
	animationRepository.ClearAssets();
};
