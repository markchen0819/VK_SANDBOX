#pragma once
#include "../../Imgui/ImGuiManager.h"
namespace IHCEngine::Core
{
	class ImGuiManagerLocator
	{
	public:
		static IMGUI::ImGuiManager* GetImGuiManager()
		{
			return systemInstance;
		}

		static void Provide(IMGUI::ImGuiManager* system)
		{
			systemInstance = system;
		}

	private:
		inline static IMGUI::ImGuiManager* systemInstance{ nullptr };
	};
}