#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_PhysicallyBasedSimulationViewer : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;

    private:

        bool reloadScene = false;
        bool startedSimulating = false;

        const char* integrationMethodNames[4] = { "Euler", "Verlet", "RungeKutta2", "RungeKutta4" };

        // Current item in the combo box
        int currentItem = 3;


        bool isP1Pinned = true;
        bool isP2Pinned = true;
        bool isP3Pinned = true;
        bool isP4Pinned = true;

        float windstrength = 0.0;
    };
}

