#include "../../Application/src/IApplication.h"

// Forward declaration
namespace IHCEngine
{
	namespace Window
	{
		class AppWindow;
	}
	namespace Core
	{
		class AssetManager;
		class SceneManager;
	}
	namespace Graphics
	{
		class GraphicsManager;
	}
	namespace Component
	{
		class CustomBehaviorManager;
	}
}

namespace IHCEngine::Core
{
	class Engine
	{
	public:
		Engine();
		~Engine();
		void Init();
		void Update();
		void Shutdown();

		void SetApplication(std::shared_ptr<IApplication> application)
		{
			this->application = application;
		}

	private:
		
		std::shared_ptr<IApplication> application;

		// Engine subsystems
		std::unique_ptr<IHCEngine::Window::AppWindow> appWindow;
		std::unique_ptr<IHCEngine::Core::AssetManager> assetManager;
		std::unique_ptr<IHCEngine::Core::SceneManager> sceneManager;
		std::unique_ptr<IHCEngine::Graphics::GraphicsManager> graphicsManager;
		std::unique_ptr<IHCEngine::Component::CustomBehaviorManager> customBehaviorManager;
	};
}
