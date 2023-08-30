#include "../../Application/src/IApplication.h"
#include "Time/Time.h"
#include "../Window/AppWindow.h"
#include "Asset/AssetManager.h"
#include "Scene/SceneManager.h"
#include "../Graphics/GraphicsManager.h"



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

		// Engine subsystems
		std::unique_ptr<IHCEngine::Window::AppWindow> appWindow;
		std::unique_ptr<IHCEngine::Core::AssetManager> assetManager;
		std::unique_ptr<IHCEngine::Core::SceneManager> sceneManager;
		std::unique_ptr<IHCEngine::Graphics::GraphicsManager> graphicsManager;
	};
}
