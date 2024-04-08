#pragma once

#include "DescriptorWraps/GlobalDescriptorWrap.h"
#include "DescriptorWraps/TextureDescriptorWrap.h"
#include "DescriptorWraps/SkeletalDescriptorWrap.h"
#include "DescriptorWraps/ComputeParticleDescriptorWrap.h"
#include "DescriptorWraps/ComputeGrassDescriptorWrap.h"
#include "DescriptorWraps/ComputeFluidDescriptorWrap.h"

namespace IHCEngine::Component
{
	class ComputeFluidComponent;
	class ComputeGrassComponent;
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
        ComputeGrassDescriptorWrap* GetComputeGrassDescriptorWrap() const { return computeGrassDescriptorWrap.get(); }
        ComputeFluidDescriptorWrap* GetComputeFluidDescriptorWrap() const { return computeFluidDescriptorWrap.get(); }

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

        void AllocateComputeGrassDescriptorSet(Component::ComputeGrassComponent* computeGrass);
        void DeallocateComputeGrassDescriptorSet(Component::ComputeGrassComponent* computeGrass);

        void AllocateComputeFluidDescriptorSet(Component::ComputeFluidComponent* computeFluid);
        void DeallocateComputeFluidDescriptorSet(Component::ComputeFluidComponent* computeFluid);
	private:

        IHCDevice& ihcDevice;
        std::unique_ptr<GlobalDescriptorWrap> globalDescriptorWrap = nullptr;
        std::unique_ptr<TextureDescriptorWrap> textureDescriptorWrap = nullptr;
        std::unique_ptr<SkeletalDescriptorWrap> skeletalDescriptorWrap = nullptr;
        std::unique_ptr<ComputeParticleDescriptorWrap> computeParticleDescriptorWrap = nullptr;
        std::unique_ptr<ComputeGrassDescriptorWrap> computeGrassDescriptorWrap = nullptr;
        std::unique_ptr<ComputeFluidDescriptorWrap> computeFluidDescriptorWrap = nullptr;
	};
}


// Analogy
// Buffers: (store ingredients)
// DescriptorSetLayout: (recipe, how to assemble the ingredients)
// DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
// DescriptorSets: Dishes on the preparation table

