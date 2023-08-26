#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#include "Engine/src/Core/Engine.h"
#include "EntryPoint.h"
#include "Application/src/TestApplication.h"


int main()
{

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    // Test CrtDEBUG getting leak messages
    //int* test = new int(); // {223} normal block at 0x000001673819B570, 4 bytes long.

    IHCEngine::Core::Engine engine;
   // std::shared_ptr<TestApplication> test;
   // engine.SetApplication(test);
    engine.Init();
    engine.Update();
    engine.Shutdown();

	return 0;
}