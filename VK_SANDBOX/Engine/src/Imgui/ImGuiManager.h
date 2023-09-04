#pragma once

namespace IHCEngine::IMGUI
{
	class ImGuiManager
	{
	public:
		void Init();  // window & graphics locator required
		void ShutDown();
		void NewFrame();
		// ImGui::Render(); called inside GraphicsManager

	private:
		void createImguiDescriptorPool();
		void updateContent();
		VkDescriptorPoolCreateInfo pool_info;
		VkDescriptorPool imguiPool;
	};


}
