#pragma once
#include "AssetRepository.h"
#include "../../Graphics/VKWraps/IHCTexture.h"
#include "../../Graphics/VKWraps/IHCMesh.h"
#include "../../Graphics/Animation/Model.h"

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
		AssetRepository<IHCEngine::Graphics::IHCMesh>& GetMeshRepository() { return  meshRepository; };
		AssetRepository<IHCEngine::Graphics::Model>& GetModelRepository() { return modelRepository; };

	private:
		AssetRepository<IHCEngine::Graphics::IHCTexture> textureRepository;
		AssetRepository<IHCEngine::Graphics::IHCMesh> meshRepository;
		AssetRepository<IHCEngine::Graphics::Model> modelRepository;
	};
}
