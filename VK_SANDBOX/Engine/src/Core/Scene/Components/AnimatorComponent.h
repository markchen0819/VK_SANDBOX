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

		std::vector<Vertex>& GetDebugBoneVertices();
		Graphics::IHCBuffer* GetDebugBoneBuffer();

	private:

		IHCEngine::Graphics::Animator animator;

		std::unique_ptr<Graphics::IHCBuffer> debugBoneBuffer;

		void Remove() override;
	};
}
