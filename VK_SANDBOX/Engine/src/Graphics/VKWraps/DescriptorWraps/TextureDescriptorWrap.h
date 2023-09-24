#pragma once
#include "IHCDescriptorWrap.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;

	class TextureDescriptorWrap : public IHCDescriptorWrap
	{
	public:
		TextureDescriptorWrap(IHCDevice& device);
		~TextureDescriptorWrap() override;
		void Setup() override;
		void CreateResources() override;
		void AllocAndBindDescriptorSets() override;

	private:

		const int TEXTURE_COUNT_LIMIT = 200;
	};
}
