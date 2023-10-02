#pragma once
#include "Component.h"


namespace IHCEngine::Graphics
{
	class IHCMesh;
}

namespace IHCEngine::Component
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent();
		~MeshComponent()=default;

		void SetMesh(Graphics::IHCMesh* mesh);
		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:

		IHCEngine::Graphics::IHCMesh* mesh = nullptr;
		void Remove() override;
	};
}
