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


    std::shared_ptr<TestApplication::TestApplication> testApplication
        = std::make_shared<TestApplication::TestApplication>("Mark's Test App");

    IHCEngine::Core::Engine engine;
    engine.SetApplication(testApplication);
    engine.Init();
    engine.Update();
    engine.Shutdown();

	return 0;
}