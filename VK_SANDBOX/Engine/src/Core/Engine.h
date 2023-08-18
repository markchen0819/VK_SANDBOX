#include "../../Application/src/IApplication.h"
#include "../Window/AppWindow.h"
#include "../Graphics/RenderSystem.h"

namespace IHCEngine::Core
{
	class Engine
	{
	public:
		Engine();
		void Init();
		void Update();
		void Shutdown();

		void SetApplication(std::unique_ptr<IApplication>& application)
		{
			this->application = application.get();
		}

	private:
		
		IApplication* application;

		// Engine subsystems
		std::unique_ptr<IHCEngine::Window::AppWindow> appWindow;
		std::unique_ptr<IHCEngine::Graphics::RenderSystem> renderSystem;

	};
}
