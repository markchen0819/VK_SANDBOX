#pragma once
#include "../../../VKWraps/IHCPipeline.h"

namespace IHCEngine::Core
{
	class GameObject;
}

namespace IHCEngine::Graphics
{
	class IHCDevice;
	class IHCDescriptorManager;
	class IHCPipeline;
	struct FrameInfo;
}

namespace IHCEngine::Graphics
{
	// Interface
	class ICustomPipeline
	{
	public:
		virtual ~ICustomPipeline() = default;
		virtual void Render(FrameInfo& frameInfo) = 0;
	};


	// Base Class
	class CustomPipelineBase : public ICustomPipeline
	{
	public:
		CustomPipelineBase(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		virtual ~CustomPipelineBase();
		void AddGameObjectToRender(Core::GameObject* gobj);
		void RemoveGameObjectToRender(Core::GameObject* gobj);

	protected:
		IHCEngine::Graphics::IHCDevice& ihcDevice;
		const IHCDescriptorManager* descriptorManager;
		VkPipelineLayout pipelineLayout; // (CPU - shader stages, uniform buffers, samplers, and push constants)
		std::unique_ptr<IHCEngine::Graphics::IHCPipeline> pipeline;
		VkRenderPass renderPass;
		std::vector<IHCEngine::Core::GameObject*> gameObjects;

		virtual void createLayout() = 0;
		virtual void createPipeline() = 0;
		virtual void destroyPipeline() = 0;
	};
}


