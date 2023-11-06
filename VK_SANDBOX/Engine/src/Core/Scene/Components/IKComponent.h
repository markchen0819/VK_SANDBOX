#pragma once
#include "Component.h"
#include "../../../Graphics/Animation/InverseKinematicsSolver.h"

namespace IHCEngine::Graphics
{
	struct FrameInfo;
	class IHCBuffer;
}

namespace IHCEngine::Component
{
	class IKComponent : public Component
	{
	public:
		IKComponent();
		~IKComponent() = default;

		void Update();
		void SetModel(Graphics::Model* m);
		std::vector<glm::mat4>& GetFinalBoneMatrices();


		// Vulkan
		std::vector<VkDescriptorSet>& GetDescriptorSets();
		std::vector<Graphics::IHCBuffer*>& GetBuffers();
		// Debug
		void AllocateDebugBoneBuffer();
		void UpdateDebugBoneBuffer(Graphics::FrameInfo& frameInfo);
		void DrawDebugBoneBuffer(Graphics::FrameInfo& frameInfo);

	private:

		IHCEngine::Graphics::InverseKinematicsSolver IKSolver;

		void Remove() override;
	};
}
