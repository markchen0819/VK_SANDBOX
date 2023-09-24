#pragma once
#include "Component.h"
#include "../../../Graphics/Animation/Animator.h"


namespace IHCEngine::Component
{
	class AnimatorComponent : public Component
	{
	public:
		AnimatorComponent();
		~AnimatorComponent() = default;

		void PlayAnimation(Graphics::Animation* animation);
		bool HasAnimation();

		void UpdateAnimation(float dt);
		std::vector<glm::mat4> GetFinalBoneMatrices();

		std::vector<VkDescriptorSet>& GetDescriptorSets();
		std::vector<Graphics::IHCBuffer*>& GetBuffers();

		// Debug
		std::vector<Vertex>& GetDebugBoneVertices();
		void UpdateDebugBoneBuffer(Graphics::FrameInfo& frameInfo);
		Graphics::IHCBuffer* GetDebugBoneBuffer(Graphics::FrameInfo& frameInfo);

	private:

		IHCEngine::Graphics::Animator animator;

		void Remove() override;
	};
}
