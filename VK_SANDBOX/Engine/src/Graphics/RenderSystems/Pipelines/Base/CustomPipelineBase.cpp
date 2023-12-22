#include "../../../../pch.h"
#include "CustomPipelineBase.h"

namespace IHCEngine::Graphics
{
	CustomPipelineBase::CustomPipelineBase(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
		: ihcDevice(device), renderPass(renderPass), descriptorManager(descriptorManager)
	{
		// Virtual functions, call them in derived classes
		//createLayout();
		//createPipeline();
	}

	CustomPipelineBase::~CustomPipelineBase()
	{
		//destroyPipeline();
	}

	void CustomPipelineBase::AddGameObjectToRender(Core::GameObject* gobj)
	{
		// Check if gobj is already in the gameObjects vector
		auto it = std::find(gameObjects.begin(), gameObjects.end(), gobj);
		if (it == gameObjects.end()) 
		{
			gameObjects.push_back(gobj);
		}
	}

	void CustomPipelineBase::RemoveGameObjectToRender(Core::GameObject* gobj)
	{
		auto it = std::find(gameObjects.begin(), gameObjects.end(), gobj);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
		}
		else
		{
			//std::cerr << "Removing object doesn't exist" << std::endl;
		}
	}
}