#pragma once
#include "AssetRepository.h"
#include "../../Graphics/VKWraps/IHCTexture.h"
#include "../../Graphics/VKWraps/IHCModel.h"

namespace IHCEngine::Core
{
	class AssetManager
	{
	public:
		AssetManager() = default;
		~AssetManager();

		// no duplication
		AssetManager(const  AssetManager&) = delete;
		AssetManager& operator=(const  AssetManager&) = delete;

		AssetRepository<IHCEngine::Graphics::IHCTexture>& GetTextureRepository() { return textureRepository; };
		AssetRepository<IHCEngine::Graphics::IHCModel>& GetModelRepository() { return  modelRepository; };

	private:
		AssetRepository<IHCEngine::Graphics::IHCTexture> textureRepository;
		AssetRepository<IHCEngine::Graphics::IHCModel> modelRepository;
	};
}
