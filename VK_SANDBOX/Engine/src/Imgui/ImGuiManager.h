#pragma once

namespace IHCEngine::Component
{
	class ImguiContextComponent;
}

namespace IHCEngine::IMGUI
{
	class ImGuiManager
	{
	public:
		void Init();  // window & graphics locator required
		void ShutDown();
		void NewFrame();
		// ImGui::Render(); called inside GraphicsManager

		void RegisterImguiContextComponent(Component::ImguiContextComponent* c);
		void RemoveImguiContextComponent(Component::ImguiContextComponent* c);

	private:
		void createImguiDescriptorPool();
		void updateContent();
		VkDescriptorPoolCreateInfo pool_info;
		VkDescriptorPool imguiPool;

		std::vector<Component::ImguiContextComponent*> contextComponents;
	};


}
