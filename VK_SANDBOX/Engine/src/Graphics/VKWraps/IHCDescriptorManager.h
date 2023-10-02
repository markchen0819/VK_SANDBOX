#pragma once
#include "DescriptorWraps/GlobalDescriptorWrap.h"
#include "DescriptorWraps/TextureDescriptorWrap.h"
#include "DescriptorWraps/SkeletalDescriptorWrap.h"

namespace IHCEngine::Graphics
{
    class IHCDevice;
	class Animator;
	class IHCTexture;

    // For accessing between shader(pipeline) and resources
	class IHCDescriptorManager
	{
	public:

        IHCDescriptorManager(IHCDevice& ihcDevice);
        ~IHCDescriptorManager()=default;

        // For rendersystem
        GlobalDescriptorWrap* GetGlobalDescriptorWrap() const { return globalDescriptorWrap.get(); }
        TextureDescriptorWrap* GetTextureDescriptorWrap() const { return textureDescriptorWrap.get(); }
        SkeletalDescriptorWrap* GetSkeletalDescriptorWrap() const { return skeletalDescriptorWrap.get(); }

        // Get Layouts for pipeline creation
        std::vector<VkDescriptorSetLayout> GetDefaultDescriptorSetLayoutsForBasicRenderSystem();

        void AllocateTextureDescriptorSetForTexture(IHCTexture* texture);
        void DeallocateTextureDescriptorSetForTexture(IHCTexture* texture);
        void AllocateSkeletalDescriptorSetForAnimator(Animator* animator);
        void DeallocateSkeletalDescriptorSetForAnimator(Animator* animator);


	private:

        IHCDevice& ihcDevice;
        std::unique_ptr<GlobalDescriptorWrap> globalDescriptorWrap = nullptr;
        std::unique_ptr<TextureDescriptorWrap> textureDescriptorWrap = nullptr;
        std::unique_ptr<SkeletalDescriptorWrap> skeletalDescriptorWrap = nullptr;

	};
}


// Analogy
// Buffers: (store ingredients)
// DescriptorSetLayout: (recipe, how to assemble the ingredients)
// DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
// DescriptorSets: Dishes on the preparation table

