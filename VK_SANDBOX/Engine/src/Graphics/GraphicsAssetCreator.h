#pragma once
#include "VKWraps/IHCMesh.h"

namespace IHCEngine::Graphics
{
	class IHCDescriptorManager;
	class IHCTexture;
	class IHCMesh;
	class Model;


	class GraphicsAssetCreator
	{
	public:

		GraphicsAssetCreator(IHCDevice& ihcDevice, IHCDescriptorManager* descriptorManager);
		~GraphicsAssetCreator() = default;

		IHCTexture* CreateTexture(std::string assetName, std::string path);
		void DestroyTexture(std::string assetName);

		IHCMesh* CreateMesh(std::string assetName, std::string path);
		IHCMesh* CreateMesh(std::string assetName, IHCMesh::Builder& builder);
		void DestroyMesh(std::string assetName);

		Model* CreateModel(std::string assetName, std::string path);
		void DestroyModel(std::string assetName);

	private:

		IHCDevice& ihcDevice;
		IHCDescriptorManager* descriptorManager = nullptr;
	};
}

