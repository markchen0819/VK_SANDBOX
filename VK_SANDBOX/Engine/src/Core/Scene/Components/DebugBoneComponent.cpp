#include "../../../pch.h"
#include "DebugBoneComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/RenderSystems/RenderSystem.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"

namespace IHCEngine::Component
{
	DebugBoneComponent::DebugBoneComponent()
		:Component(ComponentType::DebugBone)
	{
		debugBoneBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	}

	void DebugBoneComponent::AllocateDebugBoneBuffer(std::vector<Vertex>& debugBoneVertices)
	{
		// Bind Pose debug vertices wasn't created, check if animation is set on the animator
		// assert(debugBoneVertices.size() != 0);

		auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
		std::vector<Vertex>& bonevertices = debugBoneVertices;
		auto vertexCount = static_cast<uint32_t>(bonevertices.size());
		uint32_t vertexSize = sizeof(bonevertices[0]);

		vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

		for (int i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			debugBoneBuffers[i] = std::make_unique<Graphics::IHCBuffer>(
				*graphicsManager->GetIHCDevice(),
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			debugBoneBuffers[i]->Map();
		}
	}

	void DebugBoneComponent::UpdateDebugBoneBuffer(std::vector<Vertex>& debugBoneVertices, Graphics::FrameInfo& frameInfo)
	{
		debugBoneBuffers[frameInfo.frameIndex]->WriteToBuffer((void*)debugBoneVertices.data());
		debugBoneBuffers[frameInfo.frameIndex]->Flush();
	}

	void DebugBoneComponent::DrawDebugBoneBuffer(std::vector<Vertex>& debugBoneVertices, Graphics::FrameInfo& frameInfo)
	{
		VkBuffer buffers[] = { debugBoneBuffers[frameInfo.frameIndex]->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, buffers, offsets);
		vkCmdDraw(frameInfo.commandBuffer, static_cast<uint32_t>(debugBoneVertices.size()), 1, 0, 0);
	}

	void DebugBoneComponent::Attach()
	{
		Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().AddGameObjectToRender(this->gameObject, Graphics::PipelineType::DEBUGBONE);
	}

	void DebugBoneComponent::Remove()
	{
		Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().RemoveGameObjectToRender(this->gameObject, Graphics::PipelineType::DEBUGBONE);
	}
}
