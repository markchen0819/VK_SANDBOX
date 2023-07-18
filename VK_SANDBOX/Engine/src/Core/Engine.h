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

		void SetApplication(std::shared_ptr<IApplication> application)
		{
			this->application = application;
		}

	private:
		std::shared_ptr<IApplication> application;

		// All non-singleton engine subsystems
		std::shared_ptr<IHCEngine::Window::AppWindow> appWindow;
		std::shared_ptr<IHCEngine::Graphics::RenderSystem> renderSystem;

	};
}
