#pragma once
#include "DescriptorWraps/ComputeParticleDescriptorWrap.h"
#include "DescriptorWraps/GlobalDescriptorWrap.h"
#include "DescriptorWraps/TextureDescriptorWrap.h"
#include "DescriptorWraps/SkeletalDescriptorWrap.h"

namespace IHCEngine::Component
{
	class ComputeParticleComponent;
}

namespace IHCEngine::Graphics
{
    class IHCDevice;
	class Animator;
    class InverseKinematicsSolver;
	class IHCTexture;

    // For accessing between shader(pipeline) and resources
	class IHCDescriptorManager
	{
	public:

        IHCDescriptorManager(IHCDevice& ihcDevice);
        ~IHCDescriptorManager()=default;

        // For render system
        GlobalDescriptorWrap* GetGlobalDescriptorWrap() const { return globalDescriptorWrap.get(); }
        TextureDescriptorWrap* GetTextureDescriptorWrap() const { return textureDescriptorWrap.get(); }
        SkeletalDescriptorWrap* GetSkeletalDescriptorWrap() const { return skeletalDescriptorWrap.get(); }

        // For particle system
		ComputeParticleDescriptorWrap* GetComputeParticleDescriptorWrap() const { return computeParticleDescriptorWrap.get(); }

        // Get Layouts for pipeline creation
        std::vector<VkDescriptorSetLayout> GetDefaultDescriptorSetLayoutsForBasicRenderSystem();

        void AllocateTextureDescriptorSetForTexture(IHCTexture* texture);
        void DeallocateTextureDescriptorSetForTexture(IHCTexture* texture);

        void AllocateSkeletalDescriptorSetForAnimator(Animator* animator);
        void DeallocateSkeletalDescriptorSetForAnimator(Animator* animator);
        void AllocateSkeletalDescriptorSetForIK(InverseKinematicsSolver* ikSolver);
        void DeallocateSkeletalDescriptorSetForIK(InverseKinematicsSolver* ikSolver);

        void AllocateComputeParticleDescriptorSet(Component::ComputeParticleComponent* computeParticle);
        void DeallocateComputeParticleDescriptorSet(Component::ComputeParticleComponent* computeParticle);

	private:

        IHCDevice& ihcDevice;
        std::unique_ptr<GlobalDescriptorWrap> globalDescriptorWrap = nullptr;
        std::unique_ptr<TextureDescriptorWrap> textureDescriptorWrap = nullptr;
        std::unique_ptr<SkeletalDescriptorWrap> skeletalDescriptorWrap = nullptr;
        std::unique_ptr<ComputeParticleDescriptorWrap> computeParticleDescriptorWrap = nullptr;
	};
}


// Analogy
// Buffers: (store ingredients)
// DescriptorSetLayout: (recipe, how to assemble the ingredients)
// DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
// DescriptorSets: Dishes on the preparation table

