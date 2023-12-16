#pragma once
#include "Component.h"

#include "../../../Graphics/VKWraps/VKHelpers.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;
	struct FrameInfo;
}

namespace IHCEngine::Component
{
	class DebugBoneComponent : public Component
	{
	public:
		DebugBoneComponent();
		~DebugBoneComponent() = default;

		// Allocate using debugBoneVertices from the Animator or IK component
		void AllocateDebugBoneBuffer(std::vector<Vertex>& debugBoneVertices);
		void UpdateDebugBoneBuffer(std::vector<Vertex>& debugBoneVertices, Graphics::FrameInfo& frameInfo);
		void DrawDebugBoneBuffer(std::vector<Vertex>& debugBoneVertices, Graphics::FrameInfo& frameInfo);

	private:
		std::vector<std::unique_ptr<Graphics::IHCBuffer>> debugBoneBuffers;

		void Attach() override;
		void Remove() override;
	};
}
