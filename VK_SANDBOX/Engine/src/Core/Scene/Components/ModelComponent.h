#pragma once

#include "Component.h"
#include "../../../Graphics/Animation/MaterialData.h"

namespace IHCEngine::Graphics
{
	class IHCMesh;
	class Model;
}

namespace IHCEngine::Component
{
	class ModelComponent : public Component
	{
	public:

		ModelComponent();
		~ModelComponent() = default;

		void SetModel(Graphics::Model* model);
		std::unordered_map<std::string, Graphics::IHCMesh*> GetMeshes();
		IHCEngine::Graphics::MaterialData GetMaterialForMesh(std::string key);

	private:

		IHCEngine::Graphics::Model* model = nullptr;
		void Attach() override;
		void Remove() override;
	};
}

