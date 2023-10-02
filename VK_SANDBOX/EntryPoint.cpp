#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "EntryPoint.h"
#include "Engine/src/Core/Engine.h"
#include "Application/src/SampleApplication/SampleApplication.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
int main()
{
    Assimp::Importer importer;
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    // Sample CrtDEBUG getting leak messages
    //int* Sample = new int(); // {223} normal block at 0x000001673819B570, 4 bytes long.


    std::shared_ptr<SampleApplication::SampleApplication> SampleApplication
        = std::make_shared<SampleApplication::SampleApplication>("Mark's Sample App");

    IHCEngine::Core::Engine engine;
    engine.SetApplication(SampleApplication);
    engine.Init();
    engine.Update();
    engine.Shutdown();

	return 0;
}