#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_InverseKinematicsViewer : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;

    private:
        //const char* ikChain[12]
        //{
	       // "mixamorig:Spine",
        //    "mixamorig:Spine1",
        //    "mixamorig:Spine2",
        //	"mixamorig:RightShoulder",
        //    "mixamorig:RightArm",
        //    "mixamorig:RightForeArm",
        //    "mixamorig:RightHand",
        //    "mixamorig:RightHandIndex1",
        //    "mixamorig:RightHandIndex2",
        //    "mixamorig:RightHandIndex3",
        //    "mixamorig:RightHandIndex4",
        //    "mixamorig:RightHandIndex4_end"
        //};
        //int selectedRoot = 4;
        //int selectedEndEffector = 11;

        const char* ikMode[2]
        {
            "IK_Animation",
            "IK_Realtime"
        };
        int selectedMode = 0;
    };
}

