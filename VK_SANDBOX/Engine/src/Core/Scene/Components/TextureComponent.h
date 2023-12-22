#pragma once
#include "Component.h"

namespace IHCEngine::Graphics
{
	class IHCTexture;
}

namespace IHCEngine::Component
{
	class TextureComponent : public Component
	{
	public:
		TextureComponent();
		~TextureComponent() = default;

		void SetTexture(Graphics::IHCTexture* texture);
		std::vector<VkDescriptorSet>& GetDescriptorSets(); 

	private:

		IHCEngine::Graphics::IHCTexture* texture = nullptr;
		void Attach() override;
		void Remove() override;
	};
}
