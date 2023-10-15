#pragma once
#include "Component.h"

namespace IHCEngine::Component
{
	class ImguiContextComponent : public Component
	{
	public:

		ImguiContextComponent();
		~ImguiContextComponent() = default;

		virtual void UpdateContext() = 0;

	private:
		void Remove() override;
	};
}
