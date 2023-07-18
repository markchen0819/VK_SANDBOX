#include "Engine/src/Core/Engine.h"
#include "EntryPoint.h"
#include "Application/src/TestApplication.h"

int main()
{
#ifdef DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    IHCEngine::Core::Engine engine;

   // std::shared_ptr<TestApplication> test;
   // engine.SetApplication(test);

    engine.Init();
    engine.Update();
    engine.Shutdown();

	return 0;
}