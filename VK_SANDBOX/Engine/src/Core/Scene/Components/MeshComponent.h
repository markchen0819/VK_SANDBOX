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
		~MeshComponent() = default;

		void SetMesh(Graphics::IHCMesh* mesh);
		IHCEngine::Graphics::IHCMesh* GetMesh() const { return mesh; }

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
		void InstanceDraw(VkCommandBuffer commandBuffer, int instanceCount);

	private:

		IHCEngine::Graphics::IHCMesh* mesh = nullptr;
		void Attach() override;
		void Remove() override;
	};
}
